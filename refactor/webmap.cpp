/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Color identifiers by their equivalence classes
 *
 * $Id: webmap.cpp,v 1.1 2002/09/04 11:11:35 dds Exp $
 */

#include <iostream>
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
#include <cstdio>		// perror


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
	for (vector <string>::const_iterator i = files.begin(); i != files.end(); i++)
		cout << "file: [" << *i << "]\n";
	return (0);
}
