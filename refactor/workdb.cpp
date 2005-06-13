/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Export the workspace database as an SQL script
 *
 * $Id: workdb.cpp,v 1.20 2005/06/13 22:40:45 dds Exp $
 */

#ifdef COMMERCIAL

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <cassert>
#include <sstream>		// ostringstream
#include <cstdio>		// perror

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "ytab.h"
#include "metrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "sql.h"
#include "workdb.h"

// Our identifiers to store as a set
class Identifier {
	Eclass *ec;		// Equivalence class it belongs to
	string id;		// Identifier name
public:
	Identifier() {}
	Identifier(Eclass *e, const string &s) : ec(e), id(s) {}
	string get_id() const { return id; }
	Eclass *get_ec() const { return ec; }
	// To create nicely ordered sets
	inline bool operator ==(const Identifier b) const {
		return (this->ec == b.ec) && (this->id == b.id);
	}
	inline bool operator <(const Identifier b) const {
		int r = this->id.compare(b.id);
		if (r == 0)
			return ((unsigned)this->ec < (unsigned)b.ec);
		else
			return (r < 0);
	}
};

// Insert an equivalence classe in the database
static void
insert_eclass(Sql *db, ostream &of, Eclass *e, const string &name)
{
	// Avoid duplicate entries (could also have a dumped Eclass attr)
	static set <Eclass *> dumped;
	if (dumped.find(e) != dumped.end())
		return;
	dumped.insert(e);
	// Update metrics
	id_msum.add_unique_id(e);

	of << "INSERT INTO IDS VALUES(" <<
	(unsigned)e << ",'" <<
	name << "'," <<
	db->boolval(e->get_attribute(is_readonly)) << ',' <<
	db->boolval(e->get_attribute(is_undefined_macro)) << ',' <<
	db->boolval(e->get_attribute(is_macro)) << ',' <<
	db->boolval(e->get_attribute(is_macroarg)) << ',' <<
	db->boolval(e->get_attribute(is_ordinary)) << ',' <<
	db->boolval(e->get_attribute(is_suetag)) << ',' <<
	db->boolval(e->get_attribute(is_sumember)) << ',' <<
	db->boolval(e->get_attribute(is_label)) << ',' <<
	db->boolval(e->get_attribute(is_typedef)) << ',' <<
	db->boolval(e->get_attribute(is_enum)) << ',' <<
	db->boolval(e->get_attribute(is_function)) << ',' <<
	db->boolval(e->get_attribute(is_cscope)) << ',' <<
	db->boolval(e->get_attribute(is_lscope)) << ',' <<
	db->boolval(e->is_unused()) <<
	");\n";
	// The projects each EC belongs to
	for (unsigned j = attr_end; j < Attributes::get_num_attributes(); j++)
		if (e->get_attribute(j))
			of << "INSERT INTO IDPROJ VALUES(" <<
			(unsigned)e << ',' << j << ");\n";
}

// Add the contents of a file to the Tokens and Strings tables
// As a side-effect insert corresponding identifiers in the database
static void
file_dump(Sql *db, ostream &of, Fileid fid)
{
	string plain;
	Tokid plainstart;
	ifstream in;

	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		perror(fid.get_path().c_str());
		exit(1);
	}
	plainstart = Tokid(fid, in.tellg());
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fid, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		Eclass *ec;
		if ((ec = ti.check_ec()) && ec->is_identifier()) {
			id_msum.add_id(ec);
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			insert_eclass(db, of, ec, s);
			fid.metrics().process_id(s);
			of << "INSERT INTO TOKENS VALUES(" << fid.get_id() <<
			"," << (unsigned)ti.get_streampos() << "," <<
			(unsigned)ec << ");\n";
			if (plain.length() > 0) {
				of << "INSERT INTO REST VALUES(" <<
				fid.get_id() <<
				"," << (unsigned)plainstart.get_streampos() <<
				",'" << plain << "');\n";
				plain.erase();
			}
			plainstart = Tokid(fid, in.tellg());
		} else {
			fid.metrics().process_char((char)val);
			plain += db->escape((char)val);
		}
	}
	if (plain.length() > 0)
		of << "INSERT INTO REST VALUES(" << fid.get_id() <<
		"," << (unsigned)plainstart.get_streampos() <<
		",'" << plain << "');\n";
}

void
workdb_schema(Sql *db, ostream &of)
{
	cout <<
		// BEGIN AUTOSCHEMA
		"CREATE TABLE IDS("			// Details of interdependant identifiers appearing in the workspace
		"EID INTEGER PRIMARY KEY,"		// Unique identifier key
		"NAME " << db->varchar() << ","		// Identifier name
		"READONLY " << db->booltype() << ", "	// True if it appears in at least one read-only file
		"UNDEFMACRO " << db->booltype() << ", "	// True if it is apparantly an undefined macro
		"MACRO " << db->booltype() << ", "	// True if it a preprocessor macro
		"MACROARG " << db->booltype() << ", "	// True if it a preprocessor macro argument
		"ORDINARY " << db->booltype() << ", "	// True if it is an ordinary identifier (variable or function)
		"SUETAG " << db->booltype() << ", "	// True if it is a structure, union, or enumeration tag
		"SUMEMBER " << db->booltype() << ", "	// True if it is a structure or union member
		"LABEL " << db->booltype() << ", "	// True if it is a label
		"TYPEDEF " << db->booltype() << ", "	// True if it is a typedef
		"ENUM " << db->booltype() << ", "	// True if it is an enumeration member
		"FUN " << db->booltype() << ", "	// True if it is a function name
		"CSCOPE " << db->booltype() << ", "	// True if its scope is a compilation unit
		"LSCOPE " << db->booltype() << ", "	// True if it has linkage scope
		"UNUSED " << db->booltype() <<		// True if it is not used
		");\n"

		"CREATE TABLE TOKENS("			// Instances of identifier tokens within the source code
		"FID INTEGER,"				// File key (references FILES)
		"FOFFSET INTEGER,"			// Offset within the file
		"EID INTEGER,\n"			// Identifier key (references IDS)
		"PRIMARY KEY(FID, FOFFSET)"
		");\n"

		"CREATE TABLE REST("			// Non-identifier source code
		"FID INTEGER,"				// File key (references FILES)
		"FOFFSET INTEGER,"			// Offset within the file
		"CODE " << db->varchar() << ","		// The actual code
		"PRIMARY KEY(FID, FOFFSET)"
		");\n"

		"CREATE TABLE PROJECTS("		// Project details
		"PID INTEGER PRIMARY KEY,"		// Unique project key
		"NAME " << db->varchar() <<		// Project name
		");\n"

		"CREATE TABLE IDPROJ("			// Identifiers appearing in projects
		"EID INTEGER, "				// Identifier key (references IDS)
		"PID INTEGER"				// Project key (references PROJECTS)
		");\n"

		"CREATE TABLE FILES("			// File details
		"FID INTEGER PRIMARY KEY,"		// Unique file key
		"NAME " << db->varchar() << ",\n"	// File name
		"RO " << db->booltype() << ",\n"	// True if the file is read-only
		"NCHAR INTEGER,\n"			// Size in characters
		"NLCOMMENT INTEGER,\n"			// Number of line comments
		"NBCOMMENT INTEGER,\n"			// Number of block comments
		"NLINE INTEGER,\n"			// Number of lines
		"MAXLINELEN INTEGER,\n"			// Maximum line length
		"NCCOMMENT INTEGER,\n"			// Number of comment characters
		"NSPACE INTEGER,\n"			// Number of spaces
		"NFUNCTION INTEGER,\n"			// Number of functions
		"NPPDIRECTIVE INTEGER,\n"		// Number of C preprocessor directives
		"NINCFILE INTEGER,\n"			// Number of included files
		"NSTATEMENT INTEGER,\n"			// Number of C statements
		"NSTRING INTEGER"			// Number of strings
		");\n"

		"CREATE TABLE FILEPROJ("		// Files used in projects
		"FID INTEGER, "				// File key (references FILES)
		"PID INTEGER"				// Project key (references PROJECTS)
		");\n"

		"CREATE TABLE DEFINERS("		// Included files defining required elements for a given compilation unit and project
		"PID INTEGER, "				// Project key (references PROJECTS)
		"CUID INTEGER, "			// Compilation unit key (references FILES)
		"BASEFILEID INTEGER, "			// File requiring a definition (references FILES)
		"DEFINERID INTEGER"			// File providing a definition (references FILES)
		");\n"

		"CREATE TABLE INCLUDERS("		// Included files including required files for a given compilation unit and project
		"PID INTEGER, "				// Project key (references PROJECTS)
		"CUID INTEGER, "			// Compilation unit key (references FILES)
		"BASEFILEID INTEGER, "			// File included in the compilation (references FILES)
		"INCLUDERID INTEGER"			// Files that include it (references FILES)
		");\n"

		"CREATE TABLE PROVIDERS("		// Included files providing code or data for a given compilation unit and project
		"PID INTEGER, "				// Project key (references PROJECTS)
		"CUID INTEGER, "			// Compilation unit key (references FILES)
		"PROVIDERID INTEGER"			// Included file (references FILES)
		");\n"

		"CREATE TABLE FUNCTIONS("		// C functions and function-like macros
		"ID INTEGER PRIMARY KEY,\n"		// Unique function identifier
		"NAME " << db->varchar() << ",\n"	// Function name (redundant; see FUNCTIONID)
		"ISMACRO " << db->booltype() << ",\n"	// True if a function-like macro (otherwise a C function)
		"DEFINED " << db->booltype() << ",\n"	// True if the function is defined within the workspace
		"DECLARED " << db->booltype() << ",\n"	// True if the function is declared within the workspace
		"FILESCOPED " << db->booltype() << ",\n"// True if the function's scope is a single compilation unit (static or macro)
		"FID INTEGER,\n"			// File key of the function's definition, declaration, or use (references FILES)
		"FOFFSET INTEGER"			// Offset of definition, declaration, or use within the file
		");\n"

		"CREATE TABLE FUNCTIONID("		// Identifiers comprising a function's name
		"FUNCTIONID INTEGER, "			// Function identifier key (references FUNCTIONS)
		"ORDINAL INTEGER, "			// Position of the identifier within the function name (0-based)
		"EID INTEGER"				// Identifier key (references IDS)
		");\n"

		"CREATE TABLE FCALLS("			// Function calls
		"SOURCEID INTEGER, "			// Calling function identifier key (references FUNCTIONS)
		"DESTID INTEGER"			// Called function identifier key (references FUNCTIONS)
		");\n"
		// END AUTOSCHEMA
		"";
}

void
workdb_rest(Sql *db, ostream &of)
{
	vector <Fileid> files = Fileid::files(true);

	// Project names
	const Project::proj_map_type &m = Project::get_project_map();
	Project::proj_map_type::const_iterator pm;
	for (pm = m.begin(); pm != m.end(); pm++)
		cout << "INSERT INTO PROJECTS VALUES(" <<
		(*pm).second << ",'" << (*pm).first << "');\n";

	// Details for each file
	// As a side effect populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		file_dump(db, cout, (*i));
		cout << "INSERT INTO FILES VALUES(" <<
		(*i).get_id() << ",'" <<
		(*i).get_path() << "'," <<
		db->boolval((*i).get_readonly()) << ',' <<
		(*i).metrics().get_nchar() << ',' <<
		(*i).metrics().get_nlcomment() << ',' <<
		(*i).metrics().get_nbcomment() << ',' <<
		(*i).metrics().get_nline() << ',' <<
		(*i).metrics().get_maxlinelen() << ',' <<
		(*i).metrics().get_nccomment() << ',' <<
		(*i).metrics().get_nspace() << ',' <<
		(*i).metrics().get_nfunction() << ',' <<
		(*i).metrics().get_nppdirective() << ',' <<
		(*i).metrics().get_nincfile() << ',' <<
		(*i).metrics().get_nstatement() << ',' <<
		(*i).metrics().get_nstring() <<
		");\n";
		// The projects this file belongs to
		for (unsigned j = attr_end; j < Attributes::get_num_attributes(); j++)
			if ((*i).get_attribute(j))
				cout << "INSERT INTO FILEPROJ VALUES(" <<
				(*i).get_id() << ',' << j << ");\n";
	}
}

#endif /* COMMERCIAL */
