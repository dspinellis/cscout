/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Color identifiers by their equivalence classes
 *
 * $Id: webmap.cpp,v 1.2 2002/09/04 14:31:17 dds Exp $
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
#include <strstream>
#include <cstdio>		// perror

#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir


#include "cpp.h"
#include "ytab.h"
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

typedef deque<string> deque_string;


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

// Display the contents of a file in hypertext form
static void
file_hypertext(string fname)
{
	ifstream in;
	Fileid fi;

	in.open(fname.c_str());
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
		if ((ec = ti.check_ec()) && ec->get_size() > 1) {
			cout << "<a href=\"id" << (unsigned)ec << "\">";
			int len = ec->get_len();
			cout << (char)val;
			for (int j = 1; j < len; j++)
				cout << html((char)in.get());
			cout << "</a>";
			continue;
		}
		cout << html((char)val);
	}
	in.close();
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
		"<meta name=\"GENERATOR\" content=\"$Id: webmap.cpp,v 1.2 2002/09/04 14:31:17 dds Exp $\">\n"
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
	vector <string> files = Fileid::file_vector();
	ofstream fo;

	mkdir("html", 0777);

	// Index
	html_head(fo, "index", "Table of Contents");
	fo <<	"<ul>\n"
		"<li> <a href=\"afiles.html\">All files</a>\n"
		"<li> <a href=\"rofiles.html\">Read-only files</a>\n"
		"<li> <a href=\"wfiles.html\">Writable files</a>\n"
		"</ul>";
	html_tail(fo);

	// Read-only files
	html_head(fo, "rofiles", "Read-only Files");
	fo << "<ul>";
	for (vector <string>::const_iterator i = files.begin(); i != files.end(); i++) {
		Fileid fi = Fileid(*i);
		if (fi.get_readonly() == true) {
			fo << "\n<li>";
			html_file(fo, fi);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Writable files
	html_head(fo, "wfiles", "Writable Files");
	fo << "<ul>";
	for (vector <string>::const_iterator i = files.begin(); i != files.end(); i++) {
		Fileid fi = Fileid(*i);
		if (fi.get_readonly() == false) {
			fo << "\n<li>";
			html_file(fo, fi);
		}
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// All files
	html_head(fo, "afiles", "All Files");
	fo << "<ul>";
	for (vector <string>::const_iterator i = files.begin(); i != files.end(); i++) {
		fo << "\n<li>";
		html_file(fo, *i);
	}
	fo << "\n</ul>\n";
	html_tail(fo);

	// Each file
	for (vector <string>::const_iterator i = files.begin(); i != files.end(); i++) {
		Fileid fi = Fileid(*i);
		strstream fname;
		fname << 'f' << fi.get_id();
		string sfname(fname.str(), fname.pcount());
		html_head(fo, sfname.c_str(), html(*i));
		fo << "Read-only: " << (fi.get_readonly() ? "Yes" : "No") << "<p>\n";

		html_tail(fo);
	}


	return (0);
}
