/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * HTML utility functions
 *
 * $Id: html.cpp,v 1.1 2008/07/01 13:17:40 dds Exp $
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
#include <utility>
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#include "swill.h"
#include "getopt.h"

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "html.h"
#include "option.h"
#include "version.h"
#include "license.h"

/*
 * Return as a C string the HTML equivalent of character c
 * Handles tab-stop expansion provided all output is processed through this
 * function
 */
const char *
html(char c)
{
	static char str[2];
	static int column = 0;
	static vector<string> spaces(0);

	switch (c) {
	case '&': column++; return "&amp;";
	case '<': column++; return "&lt;";
	case '>': column++; return "&gt;";
	case '"': column++; return "&quot;";
	case ' ': column++; return "&nbsp;";
	case '\t':
		if ((int)(spaces.size()) != Option::tab_width->get()) {
			spaces.resize(Option::tab_width->get());
			for (int i = 0; i < Option::tab_width->get(); i++) {
				string t;
				for (int j = 0; j < Option::tab_width->get() - i; j++)
					t += "&nbsp;";
				spaces[i] = t;
			}
		}
		return spaces[column % Option::tab_width->get()].c_str();
	case '\n':
		column = 0;
		return "<br>\n";
	default:
		column++;
		str[0] = c;
		return str;
	}
}

// HTML-encode the given string
string
html(const string &s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += html(*i);
	return r;
}

// Output s as HTML in of
void
html_string(FILE *of, string s)
{
	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		fputs(html(*i), of);
}


// Create a new HTML file with a given filename and title
// The heading, if not given, will be the same as the title
void
html_head(FILE *of, const string fname, const string title, const char *heading)
{
	swill_title(title.c_str());
	if (DP())
		cerr << "Write to " << fname << endl;
	fprintf(of,
		"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"CScout %s - %s\">\n",
		Version::get_revision().c_str(),
		Version::get_date().c_str());
	fputs(
		"<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">"
		"<style type=\"text/css\" >"
		"<!--"
		// Unused lines
		".unused  { color: red }\n"
		// Heading for options
		".opthead { font-weight:bold; font-size:large; text-align:left; padding-top:.8em;}\n"
		// Graphical elements for the function exploration
		"table.box {\n"
		"	border-width: 0px;\n"
		"	border-spacing: 2px;\n"
		"	border-style: none;\n"
		"	border-collapse: separate;\n"
		"}\n"
		"table.box th {\n"
		"	border-width: 1px 1px 1px 1px;\n"
		"	padding: 0px;\n"
		"	border-style: solid;\n"
		"	border-color: gray;\n"
		"	width: 1em;\n"
		"}\n"
		"table.box td {\n"
		"	border-width: 0px;\n"
		"	padding-left: 0.5em;\n"
		"	border-style: none;\n"
		"}\n"
		"\n"
		"a.plain:link {\n"
		"    text-decoration: none;\n"
		"}\n"
		"a.plain:visited {\n"
		"    text-decoration: none;\n"
		"}\n"
		"table.unbox {\n"
		"	border-width: 0px;\n"
		"	border-spacing: 2px;\n"
		"	border-style: none;\n"
		"	border-collapse: separate;\n"
		"}\n"
		"table.unbox th {\n"
		"	border-width: 1px 1px 1px 1px;\n"
		"	padding: 1px;\n"
		"	border-style: none;\n"
		"	border-color: gray;\n"
		"	width: 1em;\n"
		"}\n"
		"table.unbox td {\n"
		"	border-width: 0px;\n"
		"	padding-left: 0.5em;\n"
		"	border-style: none;\n"
		"}\n"
		"-->"
		"</style>"
		"</head>", of);
	fprintf(of,
		"<title>%s</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>%s</h1>\n",
		title.c_str(),
		heading ? heading : title.c_str());
}

// And an HTML file end
void
html_tail(FILE *of)
{
	extern Attributes::size_type current_project;

	if (current_project)
		fprintf(of, "<p> <b>Project %s is currently selected</b>\n", Project::get_projname(current_project).c_str());
	fprintf(of,
		"<p>"
		"<a href=\"index.html\">Main page</a>\n"
		" &mdash; Web: "
		"<a href=\"http://www.spinellis.gr/cscout\">Home</a>\n"
		"<a href=\"http://www.spinellis.gr/cscout/doc/index.html\">Manual</a>\n"
		"<br><hr><font size=-1>CScout %s &mdash; %s",
		Version::get_revision().c_str(),
		Version::get_date().c_str());
#ifdef COMMERCIAL
	fprintf(of, " &mdash; Licensee: %s", licensee);
#else
	fprintf(of, " &mdash; Unsupported version; can only be used on open-source software.");
#endif
	fprintf(of, "</font></body></html>\n");
}

