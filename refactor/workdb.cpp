/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Export the workspace database as an SQL script
 *
 * $Id: workdb.cpp,v 1.10 2002/10/07 20:13:59 dds Exp $
 */

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

// Return SQL equivalent for the logical value v
static inline const char *
sql_bool(bool v)
{
	return v ? "true" : "false";
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

static string
sql(string s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += sql(*i);
	return r;
}

// Insert an equivalence classe in the database
static void
insert_eclass(ofstream &of, Eclass *e, const string &name)
{
	// Avoid duplicate entries (could also have a dumped Eclass attr)
	static set <Eclass *> dumped;
	if (dumped.find(e) != dumped.end())
		return;
	dumped.insert(e);
	// Update metrics
	msum.add_unique_id(e);

	of << "INSERT INTO IDS VALUES(" << 
	(unsigned)e << ",'" <<
	name << "'," <<
	sql_bool(e->get_attribute(is_readonly)) << ',' <<
	sql_bool(e->get_attribute(is_macro)) << ',' <<
	sql_bool(e->get_attribute(is_macroarg)) << ',' <<
	sql_bool(e->get_attribute(is_ordinary)) << ',' <<
	sql_bool(e->get_attribute(is_suetag)) << ',' <<
	sql_bool(e->get_attribute(is_sumember)) << ',' <<
	sql_bool(e->get_attribute(is_label)) << ',' <<
	sql_bool(e->get_attribute(is_typedef)) << ',' <<
	sql_bool(e->get_attribute(is_cscope)) << ',' <<
	sql_bool(e->get_attribute(is_lscope)) << ',' <<
	sql_bool(e->get_size() == 1) << 
	");\n";
	// The projects each EC belongs to
	for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
		if (e->get_attribute(j))
			of << "INSERT INTO IDPROJ VALUES(" << 
			(unsigned)e << ',' << j << ");\n";
}

// Add the contents of a file to the Tokens and Strings tables
// As a side-effect insert corresponding identifiers in the database
static void
file_dump(ofstream &of, Fileid fid)
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
		int val, len;

		ti = Tokid(fid, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		Eclass *ec;
		if (ec = ti.check_ec())
			msum.add_id(ec);
		// Identifiers worth marking
		if (ec && (
		    ec->get_size() > 1 || (ec->get_attribute(is_readonly) == false && (
		      ec->get_attribute(is_lscope) || 
		      ec->get_attribute(is_cscope) || 
		      ec->get_attribute(is_macro))))) {
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			insert_eclass(of, ec, s);
			fid.metrics().process_id(s);
			of << "INSERT INTO TOKENS VALUES(" << fid.get_id() <<
			"," << (unsigned)ti.get_streampos() << "," << 
			(unsigned)ec << ");\n";
			if (plain.length() > 0) {
				of << "INSERT INTO STRINGS VALUES(" << 
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
		of << "INSERT INTO STRINGS VALUES(" << fid.get_id() <<
		"," << (unsigned)plainstart.get_streampos() <<
		",'" << plain << "');\n";
}

main(int argc, char *argv[])
{
	Pdtoken t;

	Debug::db_read();
	// Pass 1: process master file loop
	Fchar::set_input(argv[1]);
	do
		t.getnext();
	while (t.get_code() != EOF);

	// Pass 2: Create the SQL script
	vector <Fileid> files = Fileid::sorted_files();
	ofstream fo("workdb.sql");
	if (!fo) {
		perror("workdb.sql");
		exit(1);
	}
	fo <<
		"DROP TABLE TOKENS;\n"
		"DROP TABLE STRINGS;\n"
		"DROP TABLE IDPROJ;\n"
		"DROP TABLE FILEPROJ;\n"
		"DROP TABLE IDS;\n"
		"DROP TABLE PROJECTS;\n"
		"DROP TABLE FILES;\n"

		"CREATE TABLE IDS(EID INTEGER PRIMARY KEY,NAME VARCHAR,"
		"READONLY \"boolean\",\n"
		"MACRO \"boolean\",\n"
		"MACROARG \"boolean\",\n"
		"ORDINARY \"boolean\",\n"
		"SUETAG \"boolean\",\n"
		"SUMEMBER \"boolean\",\n"
		"LABEL \"boolean\",\n"
		"TYPEDEF \"boolean\",\n"
		"CSCOPE \"boolean\",\n"
		"LSCOPE \"boolean\",\n"
		"UNUSED \"boolean\");\n"

		"CREATE TABLE TOKENS(FID INTEGER,FOFFSET INTEGER,EID INTEGER,\n"
		"PRIMARY KEY(FID, FOFFSET), FOREIGN KEY (EID) REFERENCES IDS);\n"

		"CREATE TABLE STRINGS(FID INTEGER,FOFFSET INTEGER,TEXT VARCHAR,"
		"PRIMARY KEY(FID, FOFFSET));\n"

		"CREATE TABLE PROJECTS(PID INTEGER PRIMARY KEY,NAME VARCHAR);\n"

		"CREATE TABLE IDPROJ(EID INTEGER ,PID INTEGER,\n"
		"FOREIGN KEY (EID) REFERENCES IDS,\n"
		"FOREIGN KEY (PID) REFERENCES PROJECTS);\n"

		"CREATE TABLE FILES(FID INTEGER PRIMARY KEY,"
		"NAME VARCHAR,\n"
		"RO \"boolean\",\n"
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
		fo << "INSERT INTO PROJECTS VALUES(" << 
		(*pm).second << ",'" << (*pm).first << "');\n";

	// Details for each file 
	// As a side effect populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		file_dump(fo, (*i));
		fo << "INSERT INTO FILES VALUES(" << 
		(*i).get_id() << ",'" <<
		(*i).get_path() << "'," <<
		sql_bool((*i).get_readonly()) << ',' <<
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
		for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
			if ((*i).get_attribute(j))
				fo << "INSERT INTO FILEPROJ VALUES(" << 
				(*i).get_id() << ',' << j << ");\n";
	}

	// Update fle metrics
	msum.summarize_files();

	ofstream mf("metrics.txt");
	mf << msum;

	return (0);
}
