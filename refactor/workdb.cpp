/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Export the workspace database as an SQL script
 *
 * $Id: workdb.cpp,v 1.1 2002/09/14 12:13:11 dds Exp $
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

// Modifiable identifiers stored as a vector
class MIdentifier : public Identifier {
	bool xfile;		// True if it crosses files
public:
	MIdentifier(Identifier i) : xfile(false), Identifier(i) {}
	MIdentifier() : xfile(false) {}
	void set_xfile(bool v) { xfile = v; }
	bool get_xfile() const { return xfile; }
};

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
		"CREATE TABLE IDS(EID INTEGER PRIMARY KEY,NAME VARCHAR)\n"
		"CREATE TABLE TOKENS(FID INTEGER,OFFSET INTEGER,EID INTEGER)\n"
		"CREATE TABLE STRINGS(FID INTEGER,OFFSET INTEGER,TEXT VARCHAR)\n"
		"CREATE TABLE FILES(FID INTEGER PRIMARY KEY,NAME VARCHAR,RO BIT)\n";

	// Details for each file 
	// As a side effect populite the EC identifier member
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		fo << "INSERT INTO FILES VALUES(" << 
		(*i).get_id() << ",'" <<
		(*i).get_path() << "'," <<
		(*i).get_readonly() << ")\n";
		file_dump(fo, (*i));
	}

	// Equivalence classes
	vector <MIdentifier> mids(ids.begin(), ids.end());
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		fo << "INSERT INTO IDS VALUES(" << 
		(unsigned)(*i).get_ec() << ",'" <<
		(*i).get_id() << "')\n";
	}

#ifdef ndef
	// Details for each identifier
	// Set xfile as a side-effect
	for (vector <MIdentifier>::iterator i = mids.begin(); i != mids.end(); i++) {
		ostringstream fname;
		fname << (unsigned)(*i).get_ec();
		html_head(fo, (string("i") + fname.str()).c_str(), string("Identifier: ") + html((*i).get_id()));
		fo << "<ul>\n";
		fo << "<li> Read-only: " << ((*i).get_ec()->get_attribute(is_readonly) ? "Yes" : "No") << "\n";
		fo << "<li> Macro: " << ((*i).get_ec()->get_attribute(is_macro) ? "Yes" : "No") << "\n";
		fo << "<li> Macro argument: " << ((*i).get_ec()->get_attribute(is_macroarg) ? "Yes" : "No") << "\n";
		fo << "<li> Ordinary identifier: " << ((*i).get_ec()->get_attribute(is_ordinary) ? "Yes" : "No") << "\n";
		fo << "<li> Tag for struct/union/enum: " << ((*i).get_ec()->get_attribute(is_suetag) ? "Yes" : "No") << "\n";
		fo << "<li> Member of struct/union: " << ((*i).get_ec()->get_attribute(is_sumember) ? "Yes" : "No") << "\n";
		fo << "<li> Label: " << ((*i).get_ec()->get_attribute(is_label) ? "Yes" : "No") << "\n";
		fo << "<li> Typedef: " << ((*i).get_ec()->get_attribute(is_typedef) ? "Yes" : "No") << "\n";
		fo << "<li> File scope: " << ((*i).get_ec()->get_attribute(is_cscope) ? "Yes" : "No") << "\n";
		fo << "<li> Project scope: " << ((*i).get_ec()->get_attribute(is_lscope) ? "Yes" : "No") << "\n";
		fo << "<li> Unused: " << ((*i).get_ec()->get_size() == 1 ? "Yes" : "No") << "\n";
		fo << "</ul>\n";
		typedef set <Fileid, fname_order> IFSet;
		IFSet ifiles = (*i).get_ec()->sorted_files();
		(*i).set_xfile(ifiles.size() > 1);
		fo << "<h2>Dependent Files (Writable)</h2>\n";
		fo << "<ul>\n";
		for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
			if ((*j).get_readonly() == false) {
				fo << "\n<li>";
				html_file(fo, (*j).get_path());
			}
		}
		fo << "</ul>\n";
		fo << "<h2>Dependent Files (All)</h2>\n";
		fo << "<ul>\n";
		for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
			fo << "\n<li>";
			html_file(fo, (*j).get_path());
		}
		fo << "</ul>\n";
		fo << "<h2>Substitution Script</h2>\n";
		fo << "<pre>\n";
		const setTokid & toks = (*i).get_ec()->get_members();
		Fileid ofid;
		for (setTokid::const_iterator j = toks.begin(); j != toks.end(); j++) {
			if (ofid != (*j).get_fileid()) {
				fo << "f " << (*j).get_fileid().get_path() << "\n";
				ofid = (*j).get_fileid();
			}
			fo << "s " << 
				(*j).get_streampos() << ' ' <<
				(*i).get_ec()->get_len() << ' ' <<
				"NEWID\n";
		}
		fo << "</pre>\n";

		html_tail(fo);
	}
#endif

	return (0);
}
