/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Export the workspace database as an SQL script
 *
 * $Id: workdb.cpp,v 1.14 2005/05/14 12:44:59 dds Exp $
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
#include "debug.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
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

class Sql {
public:
	virtual const char *booltype() { return "BOOLEAN"; }
	virtual const char *varchar() { return "CHARACTER VARYING"; }
	virtual const char *boolval(bool v);
};

class Mysql: public Sql {
public:
	const char *booltype() { return "bool"; }
	const char *varchar() { return "TEXT"; }
	const char *boolval(bool v);
};

class Hsqldb: public Sql {
public:
	const char *varchar() { return "VARCHAR"; }
};

class Postgres: public Sql {
public:
};


// Return SQL equivalent for the logical value v
inline const char *
Sql::boolval(bool v)
{
	return v ? "true" : "false";
}

inline const char *
Mysql::boolval(bool v)
{
	return v ? "1" : "0";
}

// Return SQL equivalent of character c
static char *
sql(char c)
{
	static char str[2];

	switch (c) {
	case '\'': return "''";
	default:
		str[0] = c;
		return str;
	}
}

string
sql(string s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += sql(*i);
	return r;
}

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
			plain += sql((char)val);
		}
	}
	if (plain.length() > 0)
		of << "INSERT INTO REST VALUES(" << fid.get_id() <<
		"," << (unsigned)plainstart.get_streampos() <<
		",'" << plain << "');\n";
}

int
workdb(const char *dbengine)
{
	Sql *db;

	if (strcmp(dbengine, "mysql") == 0)
		db = new Mysql();
	else if (strcmp(dbengine, "hsqldb") == 0)
		db = new Hsqldb();
	else if (strcmp(dbengine, "postgres") == 0)
		db = new Postgres();
	else {
		cerr << "Unknown database engine " << dbengine << "\n";
		cerr << "Supported database engine types are: mysql postgres hsqldb\n";
		return 1;
	}

	// Pass 2: Create the SQL script
	vector <Fileid> files = Fileid::files(true);

	cout <<
		"CREATE TABLE IDS(EID INTEGER PRIMARY KEY,NAME " << db->varchar() << ","
		"READONLY " << db->booltype() << ", "
		"UNDEFMACRO " << db->booltype() << ", "
		"MACRO " << db->booltype() << ", "
		"MACROARG " << db->booltype() << ", "
		"ORDINARY " << db->booltype() << ", "
		"SUETAG " << db->booltype() << ", "
		"SUMEMBER " << db->booltype() << ", "
		"LABEL " << db->booltype() << ", "
		"TYPEDEF " << db->booltype() << ", "
		"ENUM " << db->booltype() << ", "
		"FUN " << db->booltype() << ", "
		"CSCOPE " << db->booltype() << ", "
		"LSCOPE " << db->booltype() << ", "
		"UNUSED " << db->booltype() <<
		");\n"

		"CREATE TABLE TOKENS(FID INTEGER,FOFFSET INTEGER,EID INTEGER,\n"
		"PRIMARY KEY(FID, FOFFSET), FOREIGN KEY (EID) REFERENCES IDS);\n"

		"CREATE TABLE REST(FID INTEGER,FOFFSET INTEGER,CODE " << db->varchar() << ","
		"PRIMARY KEY(FID, FOFFSET));\n"

		"CREATE TABLE PROJECTS(PID INTEGER PRIMARY KEY,NAME " << db->varchar() << ");\n"

		"CREATE TABLE IDPROJ(EID INTEGER ,PID INTEGER,\n"
		"FOREIGN KEY (EID) REFERENCES IDS,\n"
		"FOREIGN KEY (PID) REFERENCES PROJECTS);\n"

		"CREATE TABLE FILES(FID INTEGER PRIMARY KEY,"
		"NAME " << db->varchar() << ",\n"
		"RO " << db->booltype() << ",\n"
		"NCHAR INTEGER,\n"
		"NLCOMMENT INTEGER,\n"
		"NBCOMMENT INTEGER,\n"
		"NLINE INTEGER,\n"
		"MAXLINELEN INTEGER,\n"
		"NCCOMMENT INTEGER,\n"
		"NSPACE INTEGER,\n"

		"NFUNCTION INTEGER,\n"
		"NPPDIRECTIVE INTEGER,\n"
		"NINCFILE INTEGER,\n"
		"NSTATEMENT INTEGER,\n"
		"NSTRING INTEGER);\n"

		"CREATE TABLE FILEPROJ(FID INTEGER ,PID INTEGER,\n"
		"FOREIGN KEY (FID) REFERENCES FILES,\n"
		"FOREIGN KEY (PID) REFERENCES PROJECTS);\n"
		"";

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

	return (0);
}

#endif /* COMMERCIAL */
