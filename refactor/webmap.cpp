/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Color identifiers by their equivalence classes
 *
 * $Id: webmap.cpp,v 1.13 2002/09/11 12:00:43 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <cassert>
#include <sstream>		// ostringstream
#include <cstdio>		// perror

#ifdef unix
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#else
#include <io.h>			// mkdir 
#endif


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
file_hypertext(ofstream &of, ofstream &uof, string fname)
{
	ifstream in;
	Fileid fi;
	bool has_unused = false;

	in.open(fname.c_str(), ios::binary);
	if (in.fail()) {
		perror(fname.c_str());
		exit(1);
	}
	fi = Fileid(fname);
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val, len;

		ti = Tokid(fi, in.tellg());
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
			ids.insert(i);
			html_id(of, i);
			if (ec->get_size() == 1) {
				html_id(uof, i);
				has_unused = true;
			}
		} else {
			of << html((char)val);
			uof << html((char)val);
		}
	}
	in.close();
	return has_unused;
}

// Create a new HTML file with a given filename and title
static void
html_head(ofstream &of, const string fname, const string title)
{
	of.open((string("html/") + fname + ".html").c_str(), ios::out);
	if (!of) {
		perror(fname.c_str());
		exit(1);
	}
	of <<	"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"$Id: webmap.cpp,v 1.13 2002/09/11 12:00:43 dds Exp $\">\n"
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
		"<li> <a href=\"xids.html\">File-spanning identifiers</a>\n"
		"<li> <a href=\"upids.html\">Unused Project-scoped Writable Identifiers</a>\n"
		"<li> <a href=\"ufids.html\">Unused File-scoped Writable Identifiers</a>\n"
		"<li> <a href=\"umids.html\">Unused Macro Writable Identifiers</a>\n"
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
	// As a side effect populite the EC identifier member
	for (vector <Fileid>::const_iterator i = files.begin(); i != files.end(); i++) {
		ostringstream fname;
		const string &pathname = (*i).get_path();
		fname << (*i).get_id();
		html_head(fo, (string("f") + fname.str()).c_str(), string("File: ") + html(pathname));
		fo << "<ul>\n";
		fo << "<li> Read-only: " << ((*i).get_readonly() ? "Yes" : "No") << "\n";
		// File source listing
		html_head(sfo, (string("s") + fname.str()).c_str(), string("Source: ") + html(pathname));
		if ((*i).get_readonly() == false)
			html_head(usfo, (string("u") + fname.str()).c_str(), string("Source (with unused identifiers marked): ") + html(pathname));
		bool has_unused = file_hypertext(sfo, usfo, pathname);
		html_tail(sfo);
		if ((*i).get_readonly() == false)
			html_tail(usfo);
		fo << "<li> Contains unused identifiers: " << (has_unused ? "Yes" : "No") << "\n";
		fo << "<li> <a href=\"s" << fname.str() << ".html\">Source code</a>\n";
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

	return (0);
}
