/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Export the workspace database as an SQL script
 *
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <cctype>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <sstream>		// ostringstream
#include <cstdio>		// perror

#include "cpp.h"
#include "error.h"
#include "debug.h"
#include "attr.h"
#include "parse.tab.h"
#include "metrics.h"
#include "funmetrics.h"
#include "filemetrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ctoken.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "sql.h"
#include "workdb.h"

// Tables that are disabled (by default none)
static bool disabled_tables[table_max];

// Enable output of the specified table
void
table_enable(const char *name)
{
	static bool initialized;
	static map<string, enum e_table> table_enum;


	if (!initialized) {
		// On first call disable all tables
		for (int i = 0; i < table_max; i++)
			disabled_tables[i] = true;

		table_enum["IDS"] = t_ids;
		table_enum["FILES"] = t_files;
		table_enum["TOKENS"] = t_tokens;
		table_enum["COMMENTS"] = t_comments;
		table_enum["STRINGS"] = t_strings;
		table_enum["REST"] = t_rest;
		table_enum["LINEPOS"] = t_linepos;
		table_enum["PROJECTS"] = t_projects;
		table_enum["IDPROJ"] = t_idproj;
		table_enum["FILEPROJ"] = t_fileproj;
		table_enum["DEFINERS"] = t_definers;
		table_enum["INCLUDERS"] = t_includers;
		table_enum["PROVIDERS"] = t_providers;
		table_enum["INCTRIGGERS"] = t_inctriggers;
		table_enum["FUNCTIONS"] = t_functions;
		table_enum["FUNCTIONMETRICS"] = t_functionmetrics;
		table_enum["FUNCTIONID"] = t_functionid;
		table_enum["FCALLS"] = t_fcalls;
		table_enum["FILECOPIES"] = t_filecopies;

		initialized = true;
	}

	auto f = table_enum.find(name);
	if (f == table_enum.end()) {
		cerr << "Unkown table name " << name << endl;
		exit(1);
	}
	disabled_tables[f->second] = false;
}

// Return true if the specified table is enabled
bool
table_is_enabled(enum e_table t)
{
	return !disabled_tables[t];
}


// Our identifiers to store as a set
class Identifier {
	Eclass *ec;		// Equivalence class it belongs to
	string id;		// Identifier name
public:
	Identifier() {}
	Identifier(Eclass *e, const string &s) : ec(e), id(s) {}
	string get_id() const { return id; }
	Eclass *get_ec() const { return ec; }
	// To create sets
	inline bool operator ==(const Identifier b) const {
		return (this->ec == b.ec) && (this->id == b.id);
	}
	inline bool operator <(const Identifier b) const {
		int r = this->id.compare(b.id);
		if (r == 0)
			return (this->ec < b.ec);
		else
			return (r < 0);
	}
};

// Insert an equivalence class in the database
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

	if (table_is_enabled(t_ids))
		of << "INSERT INTO IDS VALUES(" <<
		     ptr_offset(e) << ",'" <<
		     name << "'," <<
		     db->boolval(e->get_attribute(is_readonly)) << ',' <<
		     db->boolval(e->get_attribute(is_undefined_macro)) << ',' <<
		     db->boolval(e->get_attribute(is_macro)) << ',' <<
		     db->boolval(e->get_attribute(is_macro_arg)) << ',' <<
		     db->boolval(e->get_attribute(is_ordinary)) << ',' <<
		     db->boolval(e->get_attribute(is_suetag)) << ',' <<
		     db->boolval(e->get_attribute(is_sumember)) << ',' <<
		     db->boolval(e->get_attribute(is_label)) << ',' <<
		     db->boolval(e->get_attribute(is_typedef)) << ',' <<
		     db->boolval(e->get_attribute(is_enumeration)) << ',' <<
		     db->boolval(e->get_attribute(is_yacc)) << ',' <<
		     db->boolval(e->get_attribute(is_cfunction)) << ',' <<
		     db->boolval(e->get_attribute(is_cscope)) << ',' <<
		     db->boolval(e->get_attribute(is_lscope)) << ',' <<
		     db->boolval(e->is_unused()) <<
		     ");\n";
	// The projects each EC belongs to
	if (table_is_enabled(t_idproj))
		for (unsigned j = attr_end; j < Attributes::get_num_attributes(); j++)
			if (e->get_attribute(j))
				of << "INSERT INTO IDPROJ VALUES("
				    << ptr_offset(e) << ',' << j << ");\n";
}

// Chunk the input into tables
class Chunker {
private:
	fifstream &in;		// Stream we are reading from
	string table;		// Table we are chunking into
	Sql *db;		// Database interface
	ostream &of;		// Stream for writing SQL statements
	Fileid fid;		// File we are chunking
	streampos startpos;	// Starting position of current chunk
	string chunk;		// Characters accumulated in the current chunk
public:
	bool enabled;		// True if output to the table is enabled
	Chunker(fifstream &i, Sql *d, ostream &o, Fileid f) : in(i), table("REST"), db(d), of(o), fid(f), startpos(0), enabled(table_is_enabled(t_rest)) {}

	// Flush the currently collected input into the database
	// Should be called at the point where new input is expected
	void flush() {
		if (chunk.length() > 0) {
			if (enabled)
				of << "INSERT INTO " << table << " VALUES("
				    << fid.get_id()
				    << "," << (unsigned)startpos
				    << ",'" << chunk << "');\n";
			chunk.erase();
		}
		startpos = in.tellg();
	}

	// Start collecting input for a (possibly) new table
	// e specifies whether the table is enabled
	// Should be called at the point where new input is expected
	// s can be input already collected
	void start(const char *t, bool e, const string &s = string("")) {
		flush();
		table = string(t);
		startpos -= s.length();
		chunk = db->escape(s);
		enabled = e;
	}

	void start(const char *t, bool e, char c) {
		start(t, e, string(1, c));
	}

	inline void add(char c) {
		chunk += db->escape(c);
	}
};

// Add the contents of a file to the Tokens, Comments, Strings, and Rest tables
// As a side-effect insert corresponding identifiers in the database
// and populate the LineOffset table
static void
file_dump(Sql *db, ostream &of, Fileid fid)
{
	streampos bol(0);			// Beginning of line
	bool at_bol = true;
	int line_number = 1;
	enum e_cfile_state cstate = s_normal;	// C file state machine

	fifstream in;
	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		perror(fid.get_path().c_str());
		exit(1);
	}
	Chunker chunker(in, db, of, fid);
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fid, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		char c = (char)val;
		Eclass *ec;
		if (cstate != s_block_comment &&
		    cstate != s_string &&
		    cstate != s_cpp_comment &&
		    (isalnum(c) || c == '_') &&
		    (ec = ti.check_ec()) &&
		    ec->is_identifier()) {
			id_msum.add_id(ec);
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			insert_eclass(db, of, ec, s);
			Filedetails::get_pre_cpp_metrics(fid).process_id(s, ec);
			chunker.flush();
			if (table_is_enabled(t_tokens))
				of << "INSERT INTO TOKENS VALUES("
				    << fid.get_id() << ","
				    << (unsigned)ti.get_streampos() << ","
				    << ptr_offset(ec) << ");\n";
		} else {
			Filedetails::get_pre_cpp_metrics(fid).process_char(c);
			if (c == '\n') {
				at_bol = true;
				bol = in.tellg();
				line_number++;
			} else {
				if (at_bol) {
					if (table_is_enabled(t_linepos))
						of << "INSERT INTO LINEPOS VALUES("
						    << fid.get_id()
						    << "," << (unsigned)bol
						    << "," << line_number
						    << ");\n";
					at_bol = false;
				}
			}
			switch (cstate) {
			case s_normal:
				if (c == '/')
					cstate = s_saw_slash;
				else if (c == '"') {
					cstate = s_string;
					chunker.start("STRINGS", table_is_enabled(t_strings), c);
				} else if (c == '\'') {
					cstate = s_char;
					chunker.add(c);
				} else
					chunker.add(c);
				break;
			case s_char:
				chunker.add(c);
				if (c == '\'')
					cstate = s_normal;
				else if (c == '\\')
					cstate = s_saw_chr_backslash;
				break;
			case s_string:
				chunker.add(c);
				if (c == '"') {
					cstate = s_normal;
					chunker.start("REST", table_is_enabled(t_rest));
				} else if (c == '\\')
					cstate = s_saw_str_backslash;
				break;
			case s_saw_chr_backslash:
				chunker.add(c);
				cstate = s_char;
				break;
			case s_saw_str_backslash:
				chunker.add(c);
				cstate = s_string;
				break;
			case s_saw_slash:		// After a / character
				if (c == '/') {
					cstate = s_cpp_comment;
					chunker.start("COMMENTS", table_is_enabled(t_comments), "//");
				} else if (c == '*') {
					cstate = s_block_comment;
					chunker.start("COMMENTS", table_is_enabled(t_comments), "/*");
				} else {
					chunker.add('/');
					chunker.add(c);
					cstate = s_normal;
				}
				break;
			case s_cpp_comment:		// Inside C++ comment
				chunker.add(c);
				if (c == '\n') {
					cstate = s_normal;
					chunker.start("REST", table_is_enabled(t_rest));
				}
				break;
			case s_block_comment:		// Inside C block comment
				chunker.add(c);
				if (c == '*')
					cstate = s_block_star;
				break;
			case s_block_star:		// Found a * in a block comment
				chunker.add(c);
				if (c == '/') {
					cstate = s_normal;
					chunker.start("REST", table_is_enabled(t_rest));
				} else if (c != '*')
					cstate = s_block_comment;
				break;
			default:
				csassert(0);
			}
		}
	}
	chunker.flush();
}


void
workdb_schema(Sql *db, ostream &of)
{
			// BEGIN AUTOSCHEMA
	if (table_is_enabled(t_ids))
		cout <<
			"CREATE TABLE IDS("			// Details of interdependant identifiers appearing in the workspace
			"EID " << db->ptrtype() << " PRIMARY KEY,"	// Unique identifier key
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
			"YACC " << db->booltype() << ", "	// True if it is a yacc identifier
			"FUN " << db->booltype() << ", "	// True if it is a function name
			"CSCOPE " << db->booltype() << ", "	// True if its scope is a compilation unit
			"LSCOPE " << db->booltype() << ", "	// True if it has linkage scope
			"UNUSED " << db->booltype() <<		// True if it is not used
			");\n";

	if (table_is_enabled(t_files)) {
		cout <<
			"CREATE TABLE FILES("			// File details
			"FID INTEGER PRIMARY KEY,"		// Unique file key
			"NAME " << db->varchar() << ",\n"	// File name
			"RO " << db->booltype();		// True if the file is read-only
			// AUTOSCHEMA INCLUDE metrics.cpp Metrics
			// AUTOSCHEMA INCLUDE filemetrics.cpp FileMetrics
			for (int i = 0; i < FileMetrics::metric_max; i++)
				if (!Metrics::is_internal<FileMetrics>(i))
					cout << ",\n" << Metrics::get_dbfield<FileMetrics>(i) << " INTEGER";
			cout << ");\n";
	}

	if (table_is_enabled(t_tokens))
		cout <<
			"CREATE TABLE TOKENS("			// Instances of identifier tokens within the source code
			"FID INTEGER,"				// File key (references FILES)
			"FOFFSET INTEGER,"			// Offset within the file
			"EID " << db->ptrtype() << ",\n"	// Identifier key (references IDS)
			"PRIMARY KEY(FID, FOFFSET),"
			"FOREIGN KEY(FID) REFERENCES FILES(FID),"
			"FOREIGN KEY(EID) REFERENCES IDS(EID)"
			");\n";

	if (table_is_enabled(t_comments))
		cout <<
			"CREATE TABLE COMMENTS("		// Comments in the code
			"FID INTEGER,"				// File key (references FILES)
			"FOFFSET INTEGER,"			// Offset within the file
			"COMMENT " << db->varchar() << ","	// The comment, including its delimiters
			"PRIMARY KEY(FID, FOFFSET),"
			"FOREIGN KEY(FID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_strings))
		cout <<
			"CREATE TABLE STRINGS("			// Strings in the code
			"FID INTEGER,"				// File key (references FILES)
			"FOFFSET INTEGER,"			// Offset within the file
			"STRING " << db->varchar() << ","	// The string, including its delimiters
			"PRIMARY KEY(FID, FOFFSET),"
			"FOREIGN KEY(FID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_rest))
		cout <<
			"CREATE TABLE REST("			// Remaining, non-identifier source code
			"FID INTEGER,"				// File key (references FILES)
			"FOFFSET INTEGER,"			// Offset within the file
			"CODE " << db->varchar() << ","		// The actual code
			"PRIMARY KEY(FID, FOFFSET),"
			"FOREIGN KEY(FID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_linepos))
		cout <<
			"CREATE TABLE LINEPOS("			// Line number offsets within each file
			"FID INTEGER,"				// File key (references FILES)
			"FOFFSET INTEGER,"			// Offset within the file
			"LNUM INTEGER,"				// Line number (starts at 1)
			"PRIMARY KEY(FID, FOFFSET),"
			"FOREIGN KEY(FID) REFERENCES FILES(FID)"
			");\n";


	if (table_is_enabled(t_projects))
		cout <<
			"CREATE TABLE PROJECTS("		// Project details
			"PID INTEGER PRIMARY KEY,"		// Unique project key
			"NAME " << db->varchar() <<		// Project name
			");\n";

	if (table_is_enabled(t_idproj))
		cout <<
			"CREATE TABLE IDPROJ("			// Identifiers appearing in projects
			"EID " << db->ptrtype() << ","		// Identifier key (references IDS)
			"PID INTEGER,"				// Project key (references PROJECTS)
			"FOREIGN KEY(EID) REFERENCES IDS(EID),"
			"FOREIGN KEY(PID) REFERENCES PROJECTS(PID)"
			");\n";

	if (table_is_enabled(t_fileproj))
		cout <<
			"CREATE TABLE FILEPROJ("		// Files used in projects
			"FID INTEGER, "				// File key (references FILES)
			"PID INTEGER,"				// Project key (references PROJECTS)
			"FOREIGN KEY(FID) REFERENCES FILES(FID),"
			"FOREIGN KEY(PID) REFERENCES PROJECTS(PID)"
			");\n";

			/*
			 * Foreign keys for the following four tables are not specified, because it is difficult
			 * to satisfy integrity constraints: files (esp. their metrics, esp. ncopies) can't
			 * be written until the end of processing, while to conserve space, these tables are
			 * written after each file is processed.
			 * Alternatively, we can wrap inserts to these tables into
			 * SET REFERENTIAL_INTEGRITY { TRUE | FALSE } calls.
			 */
	if (table_is_enabled(t_definers))
		cout <<
			"CREATE TABLE DEFINERS("		// Included files defining required elements for a given compilation unit and project
			"PID INTEGER, "				// Project key (references PROJECTS)
			"CUID INTEGER, "			// Compilation unit key (references FILES)
			"BASEFILEID INTEGER, "			// File (often .c) requiring (using) a definition (references FILES)
			"DEFINERID INTEGER"			// File (often .h) providing a definition (references FILES)
			//"FOREIGN KEY(PID) REFERENCES PROJECTS(PID), "
			//"FOREIGN KEY(CUID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(BASEFILEID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(DEFINERID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_includers))
		cout <<
			"CREATE TABLE INCLUDERS("		// Included files including files for a given compilation unit and project
			"PID INTEGER, "				// Project key (references PROJECTS)
			"CUID INTEGER, "			// Compilation unit key (references FILES)
			"BASEFILEID INTEGER, "			// File included in the compilation (references FILES)
			"INCLUDERID INTEGER"			// Files that include it (references FILES)
			//"FOREIGN KEY(PID) REFERENCES PROJECTS(PID), "
			//"FOREIGN KEY(CUID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(BASEFILEID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(INCLUDERID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_providers))
		cout <<
			"CREATE TABLE PROVIDERS("		// Included files providing code or data for a given compilation unit and project
			"PID INTEGER, "				// Project key (references PROJECTS)
			"CUID INTEGER, "			// Compilation unit key (references FILES)
			"PROVIDERID INTEGER"			// Included file (references FILES)
			//"FOREIGN KEY(PID) REFERENCES PROJECTS(PID), "
			//"FOREIGN KEY(CUID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(PROVIDERID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_inctriggers))
		cout <<
			"CREATE TABLE INCTRIGGERS("		// Tokens requiring file inclusion for a given compilation unit and project
			"PID INTEGER, "				// Project key (references PROJECTS)
			"CUID INTEGER, "			// Compilation unit key (references FILES)
			"BASEFILEID INTEGER, "			// File requiring a definition (references FILES)
			"DEFINERID INTEGER, "			// File providing a definition (references FILES)
			"FOFFSET INTEGER, "			// Definition's offset within the providing file
			"LEN INTEGER"				// Token's length
			//"FOREIGN KEY(PID) REFERENCES PROJECTS(PID), "
			//"FOREIGN KEY(CUID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(BASEFILEID) REFERENCES FILES(FID), "
			//"FOREIGN KEY(DEFINERID) REFERENCES FILES(FID)"
			");\n";

	if (table_is_enabled(t_functions))
		cout <<
			"CREATE TABLE FUNCTIONS("		// C functions and function-like macros
			"ID " << db->ptrtype() << " PRIMARY KEY,\n"	// Unique function identifier
			"NAME " << db->varchar() << ",\n"	// Function name (redundant; see FUNCTIONID)
			"ISMACRO " << db->booltype() << ",\n"	// True if a function-like macro (otherwise a C function)
			"DEFINED " << db->booltype() << ",\n"	// True if the function is defined within the workspace
			"DECLARED " << db->booltype() << ",\n"	// True if the function is declared within the workspace
			"FILESCOPED " << db->booltype() << ",\n"// True if the function's scope is a single compilation unit (static or macro)
			"FID INTEGER,\n"			// File key of the function's definition, declaration, or use (references FILES)
			"FOFFSET INTEGER,\n"			// Offset of definition, declaration, or use within the file
			"FANIN INTEGER,\n"			// Fan-in (number of callers)
			"FOREIGN KEY(FID) REFERENCES FILES(FID)\n"
			");\n";

	if (table_is_enabled(t_functionmetrics)) {
		cout <<
			"CREATE TABLE FUNCTIONMETRICS("		// Metrics of defined functions and macros
			"FUNCTIONID " << db->ptrtype() << " PRIMARY KEY,\n";	// Function identifier key (references FUNCTIONS)
			// AUTOSCHEMA INCLUDE metrics.cpp Metrics
			// AUTOSCHEMA INCLUDE funmetrics.cpp FunMetrics
			for (int i = 0; i < FunMetrics::metric_max; i++)
				if (!Metrics::is_internal<FunMetrics>(i))
					cout << Metrics::get_dbfield<FunMetrics>(i) <<
					    (i >= FunMetrics::em_real_start ? " REAL" : " INTEGER") <<
					    ",\n";
			cout <<
			"FIDBEGIN INTEGER,\n"			// File key of the function's definition begin (references FILES)
			"FOFFSETBEGIN INTEGER,\n"		// Offset of definition begin within the file
			"FIDEND INTEGER,\n"			// File key of the function's definition end (references FILES)
			"FOFFSETEND INTEGER,\n"			// Offset of definition end within the file
			"FOREIGN KEY(FUNCTIONID) REFERENCES FUNCTIONS(ID)"
			");\n";

	}
	if (table_is_enabled(t_functionid))
		cout <<
			"CREATE TABLE FUNCTIONID("		// Identifiers comprising a function's name
			"FUNCTIONID " << db->ptrtype() << ", "	// Function identifier key (references FUNCTIONS)
			"ORDINAL INTEGER, "			// Position of the identifier within the function name (0-based)
			"EID " << db->ptrtype() << ", "		// Identifier key (references IDS)
			"PRIMARY KEY(FUNCTIONID, ORDINAL), "
			"FOREIGN KEY(FUNCTIONID) REFERENCES FUNCTIONS(ID), "
			"FOREIGN KEY(EID) REFERENCES IDS(EID)"
			");\n";

	if (table_is_enabled(t_fcalls))
		cout <<
			"CREATE TABLE FCALLS("			// Function calls
			"SOURCEID " << db->ptrtype() << ", "	// Calling function identifier key (references FUNCTIONS)
			"DESTID " << db->ptrtype() << ", "	// Called function identifier key (references FUNCTIONS)
			"FOREIGN KEY(SOURCEID) REFERENCES FUNCTIONS(ID), "
			"FOREIGN KEY(DESTID) REFERENCES FUNCTIONS(ID)"
			");\n";

	if (table_is_enabled(t_filecopies))
		cout <<
			"CREATE TABLE FILECOPIES("		// Files occuring in more than one copy
			"GROUPID INTEGER, "			// File group identifier
			"FID INTEGER, "				// Key of file belonging to a group of identical files (references FILES)
			"PRIMARY KEY(GROUPID, FID), "
			"FOREIGN KEY(FID) REFERENCES FILES(FID)"
			");\n";

			// END AUTOSCHEMA
}

void
workdb_rest(Sql *db, ostream &of)
{
	// Project names
	const Project::proj_map_type &m = Project::get_project_map();
	Project::proj_map_type::const_iterator pm;
	if (table_is_enabled(t_projects))
		for (pm = m.begin(); pm != m.end(); pm++)
			cout << "INSERT INTO PROJECTS VALUES("
				<< pm->second << ",'" << pm->first << "');\n";

	vector <Fileid> files = Fileid::files(true);

	int groupnum = 0;

	// Details and contents of each file
	// As a side effect populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		if (table_is_enabled(t_files)) {
			cout << "INSERT INTO FILES VALUES("
			    << i->get_id() << ",'"
			    << i->get_path() << "',"
			    << db->boolval(i->get_readonly());
			for (int j = 0; j < FileMetrics::metric_max; j++)
				if (!Metrics::is_internal<FileMetrics>(j))
					cout << ',' << i->get_pre_cpp_metrics().get_metric(j);
			cout << ");\n";
		}
		// This invalidates the file's metrics
		file_dump(db, cout, *i);
		// The projects this file belongs to
		for (unsigned j = attr_end; j < Attributes::get_num_attributes(); j++)
			if (i->get_attribute(j) && table_is_enabled(t_fileproj))
				cout << "INSERT INTO FILEPROJ VALUES("
				     << i->get_id() << ',' << j << ");\n";

		// Copies of the file
		const set <Fileid> &copies(i->get_identical_files());
		if (copies.size() > 1
		    && table_is_enabled(t_filecopies)
		    && copies.begin()->get_id() == i->get_id()) {
			for (set <Fileid>::const_iterator j = copies.begin(); j != copies.end(); j++)
				cout << "INSERT INTO FILECOPIES VALUES("
				     << groupnum << ',' << j->get_id()
				     << ");\n";
			groupnum++;
		}
	}
}
