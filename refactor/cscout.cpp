/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Color identifiers by their equivalence classes
 *
 * $Id: cscout.cpp,v 1.1 2002/11/09 16:58:00 dds Exp $
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

#include "swill.h"

#ifdef unix
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#else
#include <io.h>			// mkdir 
#endif


#include "cpp.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
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

// Modifiable identifiers stored as a vector
class MIdentifier : public Identifier {
	bool xfile;		// True if it crosses files
public:
	MIdentifier(Identifier i) : xfile(false), Identifier(i) {}
	MIdentifier() : xfile(false) {}
	void set_xfile(bool v) { xfile = v; }
	bool get_xfile() const { return xfile; }
};

typedef set <Fileid, fname_order> IFSet;

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

// Output s as HTML in of
static void
html_string(FILE *of, string s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += html(*i);
	fprintf(of, r.c_str());
}

// Display an identifier hyperlink
static void
html_id(FILE *of, const Identifier &i)
{
	fprintf(of, "<a href=\"i%d.html\">%s</a>",
		(unsigned)i.get_ec(),
		i.get_id().c_str());
}

static set <Identifier> ids;

// Add identifiers of the file fi into ids
// Return true if the file contains unused identifiers
static bool
file_analyze(Fileid fi)
{
	ifstream in;
	bool has_unused = false;
	const string &fname = fi.get_path();

	if (DP())
		cout << "Analyze to " << fname << "\n";
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
			// Worth identifying?
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
				if (ec->get_size() == 1)
					has_unused = true;
				continue;
			}
		}
		fi.metrics().process_char((char)val);
	}
	if (DP())
		cout << "nchar = " << fi.metrics().get_nchar() << '\n';
	in.close();
	return has_unused;
}

// Display the contents of a file in hypertext form
// Set show_unused to only mark unused identifiers
static void
file_hypertext(FILE *of, Fileid fi, bool show_unused)
{
	ifstream in;
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
				ids.insert(i);
				if (show_unused) {
					if (ec->get_size() == 1)
						html_id(of, i);
					else
						html_string(of, s);
				} else
					html_id(of, i);
				continue;
			}
		}
		fprintf(of, "%s", html((char)val));
	}
	in.close();
}

// Create a new HTML file with a given filename and title
static void
html_head(FILE *of, const string fname, const string title)
{
	swill_title(title.c_str());
	if (DP())
		cerr << "Write to " << fname << "\n";
	fprintf(of, 
		"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"$Id: cscout.cpp,v 1.1 2002/11/09 16:58:00 dds Exp $\">\n"
		"<title>%s</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>%s</h1>\n", title.c_str(), title.c_str());
}

// And an HTML file end
static void
html_tail(FILE *of)
{
	fprintf(of, 
		"<p>" 
		"<a href=\"index.html\">Main page</a>\n"
		"</body>"
		"</html>\n");
}

// Display a filename on an html file
static void
html_file(FILE *of, Fileid fi)
{
	fprintf(of, "<a href=\"f%d.html\">%s</a>",
		fi.get_id(),
		fi.get_path().c_str());
}

static void
html_file(FILE *of, string fname)
{
	Fileid fi = Fileid(fname);

	html_file(of, fi);
}

// Read-only files
void
rofiles_page(FILE *fo, vector <Fileid> *files)
{
	html_head(fo, "rofiles", "Read-only Files");
	fprintf(fo, "<ul>\n");
	for (vector <Fileid>::const_iterator i = (*files).begin(); i != (*files).end(); i++) {
		if ((*i).get_readonly() == true) {
			fprintf(fo, "\n<li>");
			html_file(fo, *i);
		}
	}
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
}

// Writable files
void
wfiles_page(FILE *fo, vector <Fileid> *files)
{
	html_head(fo, "wfiles", "Writable Files");
	fprintf(fo, "<ul>\n");
	for (vector <Fileid>::const_iterator i = (*files).begin(); i != (*files).end(); i++) {
		if ((*i).get_readonly() == false) {
			fprintf(fo, "\n<li>");
			html_file(fo, *i);
		}
	}
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
}

// All files
void
afiles_page(FILE *fo, vector <Fileid> *files)
{
	html_head(fo, "afiles", "All Files");
	fprintf(fo, "<ul>\n");
	for (vector <Fileid>::const_iterator i = (*files).begin(); i != (*files).end(); i++) {
		fprintf(fo, "\n<li>");
		html_file(fo, (*i).get_path());
	}
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
}

// All identifiers
void
aids_page(FILE *of, vector <MIdentifier> *mids)
{
	html_head(of, "aids", "All Identifiers");
	fprintf(of, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		fprintf(of, "\n<li>");
		html_id(of, *i);
	}
	fprintf(of, "\n</ul>\n");
	html_tail(of);
}

// Read-only identifiers
void
roids_page(FILE *of,  vector <MIdentifier> *mids)
{
	html_head(of, "roids", "Read-only Identifiers");
	fprintf(of, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		if ((*i).get_ec()->get_attribute(is_readonly) == true) {
			fprintf(of, "\n<li>");
			html_id(of, *i);
		}
	}
	fprintf(of, "</ul>\n");
	html_tail(of);
}

// Details for each identifier
void
identifier_page(FILE *fo, MIdentifier *i)
{
	ostringstream fname;
	fname << (unsigned)(*i).get_ec();
	html_head(fo, (string("i") + fname.str()).c_str(), string("Identifier: ") + html((*i).get_id()));
	Eclass *e = (*i).get_ec();
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> Read-only: %s\n", e->get_attribute(is_readonly) ? "Yes" : "No");
	fprintf(fo, "<li> Macro: %s\n", e->get_attribute(is_macro) ? "Yes" : "No");
	fprintf(fo, "<li> Macro argument: %s\n", e->get_attribute(is_macroarg) ? "Yes" : "No");
	fprintf(fo, "<li> Ordinary identifier: %s\n", e->get_attribute(is_ordinary) ? "Yes" : "No");
	fprintf(fo, "<li> Tag for struct/union/enum: %s\n", e->get_attribute(is_suetag) ? "Yes" : "No");
	fprintf(fo, "<li> Member of struct/union: %s\n", e->get_attribute(is_sumember) ? "Yes" : "No");
	fprintf(fo, "<li> Label: %s\n", e->get_attribute(is_label) ? "Yes" : "No");
	fprintf(fo, "<li> Typedef: %s\n", e->get_attribute(is_typedef) ? "Yes" : "No");
	fprintf(fo, "<li> File scope: %s\n", e->get_attribute(is_cscope) ? "Yes" : "No");
	fprintf(fo, "<li> Project scope: %s\n", e->get_attribute(is_lscope) ? "Yes" : "No");
	fprintf(fo, "<li> Unused: %s\n", e->get_size() == 1 ? "Yes" : "No");
	fprintf(fo, "<li> Appears in project(s): \n<ul>\n");
	for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
		if (e->get_attribute(j))
			fprintf(fo, "<li>%s\n", Project::get_projname(j).c_str());
	fprintf(fo, "</ul>\n");
	fprintf(fo, "</ul>\n");
	IFSet ifiles = e->sorted_files();
	(*i).set_xfile(ifiles.size() > 1);
	fprintf(fo, "<h2>Dependent Files (Writable)</h2>\n");
	fprintf(fo, "<ul>\n");
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
		if ((*j).get_readonly() == false) {
			fprintf(fo, "\n<li>");
			html_file(fo, (*j).get_path());
		}
	}
	fprintf(fo, "</ul>\n");
	fprintf(fo, "<h2>Dependent Files (All)</h2>\n<ul>\n");
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
		fprintf(fo, "\n<li>");
		html_file(fo, (*j).get_path());
	}
	fprintf(fo, "</ul>\n");
#ifdef ndef
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
#endif
	html_tail(fo);
}

// Writable identifiers
void
wids_page(FILE *fo, vector <MIdentifier> *mids)
{
	html_head(fo, "wids", "Writable Identifiers");
	fprintf(fo, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		if ((*i).get_ec()->get_attribute(is_readonly) == false) {
			fprintf(fo, "\n<li>");
			html_id(fo, *i);
		}
	}
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

// Cross-file writable identifiers
void
xids_page(FILE *fo, vector <MIdentifier> *mids)
{
	html_head(fo, "xids", "File-spanning Writable Identifiers");
	fprintf(fo, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		if ((*i).get_xfile() == true &&
		    (*i).get_ec()->get_attribute(is_readonly) == false) {
			fprintf(fo, "\n<li>");
			html_id(fo, *i);
		}
	}
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

// Unused project-scoped writable identifiers
void
upids_page(FILE *fo, vector <MIdentifier> *mids)
{
	html_head(fo, "upids", "Unused Project-scoped Writable Identifiers");
	fprintf(fo, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_lscope) == true &&
		    e->get_attribute(is_readonly) == false) {
			fprintf(fo, "\n<li>");
			html_id(fo, *i);
		}
	}
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

// Unused file-scoped writable identifiers
void
ufids_page(FILE *fo, vector <MIdentifier> *mids)
{
	html_head(fo, "ufids", "Unused File-scoped Writable Identifiers");
	fprintf(fo, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_cscope) == true &&
		    e->get_attribute(is_readonly) == false) {
			fprintf(fo, "\n<li>");
			html_id(fo, *i);
		}
	}
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

	// Unused macro writable identifiers
void
umids_page(FILE *fo, vector <MIdentifier> *mids)
{
	html_head(fo, "umids", "Unused Macro Writable Identifiers");
	fprintf(fo, "<ul>\n");
	for (vector <MIdentifier>::const_iterator i = (*mids).begin(); i != (*mids).end(); i++) {
		Eclass *e = (*i).get_ec();
		if (
		    e->get_size() == 1 &&
		    e->get_attribute(is_macro) == true &&
		    e->get_attribute(is_readonly) == false) {
			fprintf(fo, "\n<li>");
			html_id(fo, *i);
		}
	}
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

// Index
void
index_page(FILE *of, void *data)
{
	html_head(of, "index", "Ceescape Results");
	fprintf(of, 
		"<ul>\n"
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
		"</ul>");
	html_tail(of);
}

void
file_page(FILE *of, Fileid *i)
{
	ostringstream fname;
	const string &pathname = (*i).get_path();
	fname << (*i).get_id();
	html_head(of, (string("f") + fname.str()).c_str(), string("File: ") + html(pathname));
	fprintf(of, "<ul>\n");
	fprintf(of, "<li> Read-only: %s", (*i).get_readonly() ? "Yes" : "No");
	fprintf(of, "\n<li> Number of characters: %d", (*i).metrics().get_nchar());
	fprintf(of, "\n<li> Comment characters: %d", (*i).metrics().get_nccomment());
	fprintf(of, "\n<li> Space characters: %d", (*i).metrics().get_nspace());
	fprintf(of, "\n<li> Number of line comments: %d", (*i).metrics().get_nlcomment());
	fprintf(of, "\n<li> Number of block comments: %d", (*i).metrics().get_nbcomment());
	fprintf(of, "\n<li> Number of lines: %d", (*i).metrics().get_nline());
	fprintf(of, "\n<li> Length of longest line: %d", (*i).metrics().get_maxlinelen());
	fprintf(of, "\n<li> Number of preprocessor directives: %d", (*i).metrics().get_nppdirective());
	fprintf(of, "\n<li> Number of directly included files: %d", (*i).metrics().get_nincfile());
	fprintf(of, "\n<li> Number of defined functions: %d", (*i).metrics().get_nfunction());
	fprintf(of, "\n<li> Number of C statements: %d", (*i).metrics().get_nstatement());
	fprintf(of, "\n<li> Number of C strings: %d", (*i).metrics().get_nstring());
	fprintf(of, "\n<li> Used in project(s): \n<ul>");
	for (int j = attr_max; j < Attributes::get_num_attributes(); j++)
		if ((*i).get_attribute(j))
			fprintf(of, "<li>%s\n", Project::get_projname(j).c_str());
	fprintf(of, "</ul>\n<li> <a href=\"s%s.html\">Source code</a>\n", fname.str().c_str());
	fprintf(of, "\n<li> <a href=\"u%s.html\">Source code (with unused identifiers marked)</a>\n", fname.str().c_str());
	fprintf(of, "</ul>\n");
	html_tail(of);
}

void
source_page(FILE *of, Fileid *i)
{
	ostringstream fname;
	const string &pathname = (*i).get_path();
	fname << (*i).get_id();
	html_head(of, (string("s") + fname.str()).c_str(), string("Source: ") + html(pathname));
	file_hypertext(of, *i, false);
	html_tail(of);
}

void
unused_source_page(FILE *of, Fileid *i)
{
	ostringstream fname;
	const string &pathname = (*i).get_path();
	fname << (*i).get_id();
	html_head(of, (string("u") + fname.str()).c_str(), string("Source (with unused identifiers marked): ") + html(pathname));
	file_hypertext(of, *i, true);
	html_tail(of);
}


main(int argc, char *argv[])
{
	Pdtoken t;

	Debug::db_read();
	if (!swill_init(8080)) {
		cerr << "Couldn't initialize the SWILL server.\n";
		exit(1);
	}
	// Only localhost access
	swill_allow("127.0.0.1");

	// Pass 1: process master file loop
	Fchar::set_input(argv[1]);
	do
		t.getnext();
	while (t.get_code() != EOF);

	// Pass 2: Create web pages
	vector <Fileid> files = Fileid::sorted_files();

	swill_handle("index.html", index_page, 0);
	swill_handle("afiles.html", afiles_page, &files);
	swill_handle("rofiles.html", rofiles_page, &files);
	swill_handle("wfiles.html", wfiles_page, &files);

	// Populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		ostringstream fname;
		fname << (*i).get_id();
		bool has_unused = file_analyze(*i);
		swill_handle((string("s") + fname.str() + ".html").c_str(), source_page, &*i);
		swill_handle((string("u") + fname.str() + ".html").c_str(), unused_source_page, &*i);
		swill_handle((string("f") + fname.str() + ".html").c_str(), file_page, &*i);
	}

	vector <MIdentifier> mids(ids.begin(), ids.end());

	swill_handle("aids.html", aids_page, &mids);
	swill_handle("roids.html", roids_page, &mids);
	swill_handle("wids.html", wids_page, &mids);
	swill_handle("xids.html", xids_page, &mids);
	swill_handle("upids.html", upids_page, &mids);
	swill_handle("ufids.html", ufids_page, &mids);
	swill_handle("umids.html", umids_page, &mids);

	// Set xfile for each identifier
	for (vector <MIdentifier>::iterator i = mids.begin(); i != mids.end(); i++) {
		Eclass *e = (*i).get_ec();
		IFSet ifiles = e->sorted_files();
		(*i).set_xfile(ifiles.size() > 1);

		ostringstream fname;
		fname << (unsigned)(*i).get_ec();
		swill_handle((string("i") + fname.str() + ".html").c_str(), identifier_page, &*i);

		// Update metrics
		msum.add_unique_id((*i).get_ec());
	}


	// Update fle metrics
	msum.summarize_files();

	ofstream mf("metrics.txt");
	mf << msum;

	// Serve web pages
	for (;;)
		swill_serve();

	return (0);
}

