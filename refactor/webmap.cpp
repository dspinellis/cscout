/*
 * (C) Copyright 2001 Diomidis Spinellis.
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
 * Color identifiers by their equivalence classes
 *
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
#include <sstream>		// ostringstream
#include <cstdio>		// perror

#ifdef unix
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#else
#include <io.h>			// mkdir
#endif


#include "cpp.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
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

// Return HTML equivalent of character c
static char *
html(char c)
{
	static char str[2];

	switch (c) {
	case '&': return "&amp;";
	case '<': return "&lt;";
	case '>': return "&gt;";
	case ' ': return "&nbsp;";
	case '\t': return "&nbsp;&nbsp;&nbsp;&nbsp;";
	case '\n': return "<br>\n";
	default:
		str[0] = c;
		return str;
	}
}

static string
html(string s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += html(*i);
	return r;
}

// Display an identifier hyperlink
static void
html_id(ofstream &of, const Identifier &i)
{
	of << "<a href=\"i" << (unsigned)i.get_ec() << ".html\">" << i.get_id() << "</a>";
}

static set <Identifier> ids;

// Display the contents of a file in hypertext form
// As a side-effect add identifier into ids
// Return true if the file contains unused identifiers
static bool
file_hypertext(ofstream &of, ofstream &uof, Fileid fi, bool write_uof)
{
	ifstream in;
	bool has_unused = false;
	const string &fname = fi.get_path();

	if (DP())
		cout << "Write to " << fname << "\n";
	in.open(fname.c_str(), ios::binary);
	if (in.fail()) {
		perror(fname.c_str());
		exit(1);
	}
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val, len;

		ti = Tokid(fi, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		Eclass *ec;
		// Identifiers worth marking
		if (ec = ti.check_ec()) {
			// Update metrics
			msum.add_id(ec);
			// Worth marking?
			if (ec->get_size() > 1 || (ec->get_attribute(is_readonly) == false && (
			      ec->get_attribute(is_lscope) ||
			      ec->get_attribute(is_cscope) ||
			      ec->get_attribute(is_macro)))) {
				string s;
				s = (char)val;
				int len = ec->get_len();
				for (int j = 1; j < len; j++)
					s += (char)in.get();
				Identifier i(ec, s);
				fi.metrics().process_id(s);
				ids.insert(i);
				html_id(of, i);
				if (ec->get_size() == 1) {
					if (write_uof)
						html_id(uof, i);
					has_unused = true;
				} else if (write_uof)
						uof << html(s);
			} else {
				of << html((char)val);
				fi.metrics().process_char((char)val);
				if (write_uof)
					uof << html((char)val);
			}
		}
	}
	if (DP())
		cout << "nchar = " << fi.metrics().get_nchar() << '\n';
	in.close();
	return has_unused;
}

// Create a new HTML file with a given filename and title
static void
html_head(ofstream &of, const string fname, const string title)
{
	cout << "Writing " << title << "\n";
	if (DP())
		cout << "Write to " << fname << "\n";
	of.open((string("html/") + fname + ".html").c_str(), ios::out);
	if (!of) {
		perror(fname.c_str());
		exit(1);
	}
	of <<	"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<title>" << title << "</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>" << title << "</h1>\n";
}

// And an HTML file
static void
html_tail(ofstream &of)
{
	of <<	"<p>"
		"<a href=\"index.html\">Main page</a>\n"
		"</body>"
		"</html>";
	of.close();
}

// Display a filename on an html file
static void
html_file(ofstream &of, Fileid fi)
{
	of << "<a href=\"f" << fi.get_id() << ".html\">" << fi.get_path() << "</a>";
}

static void
html_file(ofstream &of, string fname)
{
	Fileid fi = Fileid(fname);

	html_file(of, fi);
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

	// Pass 2: Create web pages
	vector <Fileid> files = Fileid::sorted_files();
	ofstream fo, usfo, sfo;

	#ifdef unix
	(void)mkdir("html", 0777);
	#else
	(void)mkdir("html");
	#endif

	// Index
	html_head(fo, "index", "Table of Contents");
	fo <<	"<ul>\n"
		"<li> <a href=\"afiles.html\">All files</a>\n"
		"<li> <a href=\"rofiles.html\">Read-only files</a>\n"
		"<li> <a href=\"wfiles.html\">Writable files</a>\n"
		"<li> <a href=\"aids.html\">All identifiers</a>\n"
		"<li> <a href=\"roids.html\">Read-only identifiers</a>\n"
		"<li> <a href=\"wids.html\">Writable identifiers</a>\n"
		"<li> <a href=\"xids.html\">File-spanning writable identifiers</a>\n"
		"<li> <a href=\"upids.html\">Unused project-scoped writable identifiers</a>\n"
		"<li> <a href=\"ufids.html\">Unused file-scoped writable identifiers</a>\n"
		"<li> <a href=\"umids.html\">Unused macro writable identifiers</a>\n"
		"</ul>";
	html_tail(fo);

	// Read-only files
	html_head(fo, "rofiles", "Read-only Files");
	fo << "<ul>";
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		if ((*i).get_readonly() == true) {
			fo << "\n<li>";
			html_file(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Writable files
	html_head(fo, "wfiles", "Writable Files");
	fo << "<ul>";
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		if ((*i).get_readonly() == false) {
			fo << "\n<li>";
			html_file(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// All files
	html_head(fo, "afiles", "All Files");
	fo << "<ul>";
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		fo << "\n<li>";
		html_file(fo, (*i).get_path());
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Details for each file
	// As a side effect populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		ostringstream fname;
		const string &pathname = (*i).get_path();
		fname << (*i).get_id();
		html_head(fo, (string("f") + fname.str()).c_str(), string("File: ") + html(pathname));
		// File source listing
		html_head(sfo, (string("s") + fname.str()).c_str(), string("Source: ") + html(pathname));
		if ((*i).get_readonly() == false)
			html_head(usfo, (string("u") + fname.str()).c_str(), string("Source (with unused identifiers marked): ") + html(pathname));
		bool has_unused = file_hypertext(sfo, usfo, *i, !(*i).get_readonly());
		html_tail(sfo);
		if ((*i).get_readonly() == false)
			html_tail(usfo);
		fo << "<ul>\n" <<
		"<li> Read-only: " <<
		((*i).get_readonly() ? "Yes" : "No") <<
		"\n<li> Number of characters: " <<
		(*i).metrics().get_nchar() <<
		"\n<li> Comment characters: " <<
		(*i).metrics().get_nccomment() <<
		"\n<li> Space characters: " <<
		(*i).metrics().get_nspace() <<
		"\n<li> Number of line comments: " <<
		(*i).metrics().get_nlcomment() <<
		"\n<li> Number of block comments: " <<
		(*i).metrics().get_nbcomment() <<
		"\n<li> Number of lines: " <<
		(*i).metrics().get_nline() <<
		"\n<li> Length of longest line: " <<
		(*i).metrics().get_maxlinelen() <<
		"\n<li> Number of preprocessor directives: " <<
		(*i).metrics().get_nppdirective() <<
		"\n<li> Number of directly included files: " <<
		(*i).metrics().get_nincfile() <<
		"\n<li> Number of defined functions: " <<
		(*i).metrics().get_nfunction() <<
		"\n<li> Number of C statements: " <<
		(*i).metrics().get_nstatement() <<
		"\n<li> Number of C strings: " <<
		(*i).metrics().get_nstring() <<
		"\n<li> Contains unused identifiers: " <<
		(has_unused ? "Yes" : "No") <<
		"\n<li> Used in project(s): \n<ul>";
		for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
			if ((*i).get_attribute(j))
				fo << "<li>" << Project::get_projname(j) << "\n";
		fo << "</ul>\n<li> <a href=\"s"
		<< fname.str() << ".html\">Source code</a>\n";
		if (has_unused)
			fo << "<li> <a href=\"u" << fname.str() << ".html\">Source code (with unused identifiers marked)</a>\n";
		fo << "</ul>\n";
		html_tail(fo);
	}

	vector <MIdentifier> mids(ids.begin(), ids.end());
	// All identifiers
	html_head(fo, "aids", "All Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		fo << "\n<li>";
		html_id(fo, *i);
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Read-only identifiers
	html_head(fo, "roids", "Read-only Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		if ((*i).get_ec()->get_attribute(is_readonly) == true) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Details for each identifier
	// Set xfile as a side-effect
	for (vector <MIdentifier>::iterator i = mids.begin(); i != mids.end(); i++) {
		// Update metrics
		msum.add_unique_id((*i).get_ec());

		ostringstream fname;
		fname << (unsigned)(*i).get_ec();
		html_head(fo, (string("i") + fname.str()).c_str(), string("Identifier: ") + html((*i).get_id()));
		Eclass *e = (*i).get_ec();
		fo << "<ul>\n";
		fo << "<li> Read-only: " << (e->get_attribute(is_readonly) ? "Yes" : "No") << "\n";
		fo << "<li> Macro: " << (e->get_attribute(is_macro) ? "Yes" : "No") << "\n";
		fo << "<li> Macro argument: " << (e->get_attribute(is_macroarg) ? "Yes" : "No") << "\n";
		fo << "<li> Ordinary identifier: " << (e->get_attribute(is_ordinary) ? "Yes" : "No") << "\n";
		fo << "<li> Tag for struct/union/enum: " << (e->get_attribute(is_suetag) ? "Yes" : "No") << "\n";
		fo << "<li> Member of struct/union: " << (e->get_attribute(is_sumember) ? "Yes" : "No") << "\n";
		fo << "<li> Label: " << (e->get_attribute(is_label) ? "Yes" : "No") << "\n";
		fo << "<li> Typedef: " << (e->get_attribute(is_typedef) ? "Yes" : "No") << "\n";
		fo << "<li> File scope: " << (e->get_attribute(is_cscope) ? "Yes" : "No") << "\n";
		fo << "<li> Project scope: " << (e->get_attribute(is_lscope) ? "Yes" : "No") << "\n";
		fo << "<li> Unused: " << (e->get_size() == 1 ? "Yes" : "No") << "\n";
		fo << "<li> Appears in project(s): \n<ul>\n";
		for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
			if (e->get_attribute(j))
				fo << "<li>" << Project::get_projname(j) << "\n";
		fo << "</ul>\n</ul>\n";
		typedef set <Fileid, fname_order> IFSet;
		IFSet ifiles = e->sorted_files();
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
		const setTokid & toks = e->get_members();
		Fileid ofid;
		for (setTokid::const_iterator j = toks.begin(); j != toks.end(); j++) {
			if (ofid != (*j).get_fileid()) {
				fo << "f " << (*j).get_fileid().get_path() << "\n";
				ofid = (*j).get_fileid();
			}
			fo << "s " <<
				(*j).get_streampos() << ' ' <<
				e->get_len() << ' ' <<
				"NEWID\n";
		}
		fo << "</pre>\n";

		html_tail(fo);
	}
	// Writable identifiers
	html_head(fo, "wids", "Writable Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		if ((*i).get_ec()->get_attribute(is_readonly) == false) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Cross-file writable identifiers
	html_head(fo, "xids", "File-spanning Writable Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		if ((*i).get_xfile() == true &&
		    (*i).get_ec()->get_attribute(is_readonly) == false) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Unused project-scoped writable identifiers
	html_head(fo, "upids", "Unused Project-scoped Writable Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_lscope) == true &&
		    e->get_attribute(is_readonly) == false) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Unused file-scoped writable identifiers
	html_head(fo, "ufids", "Unused File-scoped Writable Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_cscope) == true &&
		    e->get_attribute(is_readonly) == false) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Unused macro writable identifiers
	html_head(fo, "umids", "Unused Macro Writable Identifiers");
	fo << "<ul>";
	for (vector <MIdentifier>::const_iterator i = mids.begin(); i != mids.end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_macro) == true &&
		    e->get_attribute(is_readonly) == false) {
			fo << "\n<li>";
			html_id(fo, *i);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Update fle metrics
	msum.summarize_files();

	ofstream mf("metrics.txt");
	mf << msum;

	return (0);
}
