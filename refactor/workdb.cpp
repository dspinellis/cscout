/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Export the workspace database as an SQL script
 *
 * $Id: workdb.cpp,v 1.2 2002/09/14 17:52:42 dds Exp $
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
#include "fileid.h"
#include "attr.h"
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

static set <Identifier> ids;

// Add the contents of a file to the Tokens and Strings tables
// As a side-effect add identifier into ids
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
		// Identifiers worth marking
		if ((ec = ti.check_ec()) && (
		    ec->get_size() > 1 || (ec->get_attribute(is_readonly) == false && (
		      ec->get_attribute(is_lscope) || 
		      ec->get_attribute(is_cscope) || 
		      ec->get_attribute(is_macro))))) {
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			Identifier i(ec, s);
			of << "INSERT INTO TOKENS VALUES(" << fid.get_id() <<
			"," << (unsigned)ti.get_streampos() << "," << 
			(unsigned)ec << ")\n";
			ids.insert(i);
			if (plain.length() > 0) {
				of << "INSERT INTO STRINGS VALUES(" << 
				fid.get_id() <<
				"," << (unsigned)plainstart.get_streampos() <<
				",'" << plain << "')\n";
				plain.clear();
			}
			plainstart = Tokid(fid, in.tellg());
		} else {
			plain += sql((char)val);
		}
	}
	if (plain.length() > 0)
		of << "INSERT INTO STRINGS VALUES(" << fid.get_id() <<
		"," << (unsigned)plainstart.get_streampos() <<
		",'" << plain << "')\n";
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
		"CREATE TABLE IDS(EID INTEGER PRIMARY KEY,NAME VARCHAR,"
		"readonly BIT, \n"
		"macro BIT, \n"
		"macroarg BIT, \n"
		"ordinary BIT, \n"
		"suetag BIT, \n"
		"sumember BIT, \n"
		"label BIT, \n"
		"typedef BIT, \n"
		"cscope BIT, \n"
		"lscope BIT, \n"
		"unused BIT)\n"
		"CREATE TABLE TOKENS(FID INTEGER,OFFSET INTEGER,EID INTEGER)\n"
		"CREATE TABLE STRINGS(FID INTEGER,OFFSET INTEGER,TEXT VARCHAR)\n"
		"CREATE TABLE FILES(FID INTEGER PRIMARY KEY,NAME VARCHAR,RO BIT)\n";

	// Details for each file 
	// As a side effect populate the EC identifier member
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		fo << "INSERT INTO FILES VALUES(" << 
		(*i).get_id() << ",'" <<
		(*i).get_path() << "'," <<
		sql_bool((*i).get_readonly()) << ")\n";
		file_dump(fo, (*i));
	}

	// Equivalence classes
	for (set <Identifier>::const_iterator i = ids.begin(); i != ids.end(); i++) {
		fo << "INSERT INTO IDS VALUES(" << 
		(unsigned)(*i).get_ec() << ",'" <<
		(*i).get_id() << "'," <<
		sql_bool((*i).get_ec()->get_attribute(is_readonly)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_macro)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_macroarg)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_ordinary)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_suetag)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_sumember)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_label)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_typedef)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_cscope)) << ',' <<
		sql_bool((*i).get_ec()->get_attribute(is_lscope)) << ',' <<
		sql_bool(((*i).get_ec()->get_size() == 1)) << 
		")\n";
	}

	return (0);
}
