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
#include <cstdio>		// perror


#include "cpp.h"
#include "metrics.h"
#include "attr.h"
#include "error.h"
#include "parse.tab.h"
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

typedef map<Eclass *, string> Colormap;

main(int argc, char *argv[])
{
	int i;
	int parse_parse();

	Debug::db_read();
	// Pass 1: scan files
	Block::enter();		// Linkage unit
	for (i = 1; i < argc; i++) {
		if (argv[i] == "-") {	// Linkage unit separator
			Block::exit();
			Block::enter();
			continue;
		}
		Block::enter();	// Compilation unit
		Fchar::set_input(argv[i]);
		Fchar::push_input("/dds/src/research/ie/refactor/wdefs.h");
		Fchar::push_input("/dds/src/research/ie/refactor/wincs.h");
		Pdtoken::macros_clear();
		if (parse_parse() != 0)
			exit(1);
		Block::exit();	// Compilation unit
	}
	Block::exit();		// Linkage unit

	// Pass 2: go through the files annotating identifiers
	deque_string color_names;
	// Some nice HTML colors
	color_names.push_back("ff0000");
	color_names.push_back("bf0000");
	color_names.push_back("00af00");
	color_names.push_back("00ef00");
	color_names.push_back("0000ff");
	color_names.push_back("bfbf00");
	color_names.push_back("00ffff");
	color_names.push_back("ff00ff");

	ifstream in;
	Fileid fi;
	Colormap cm;
	deque_string::const_iterator c = color_names.begin();
	cout << "<html><title>Identifier groups</title>\n"
		"<body bgcolor=\"#ffffff\">\n";
	for (i = 1; i < argc; i++) {
		if (argv[i] == "-") {	// Linkage unit separator
			cout << "<p><hr><p>\n";
			continue;
		}
		if (in.is_open())
			in.close();
		in.clear();		// Otherwise flags are dirty and open fails
		in.open(argv[i], ios::binary);
		if (in.fail()) {
			perror(argv[i]);
			exit(1);
		}
		cout << "<h2>" << argv[i] << "</h2>\n";
		fi = Fileid(argv[i]);
		// Go through the file character by character
		for (;;) {
			Tokid ti;
			int val, len;

			ti = Tokid(fi, in.tellg());
			if ((val = in.get()) == EOF)
				break;
			Eclass *ec;
			if ((ec = ti.check_ec()) && ec->get_size() > 1) {
				Colormap::const_iterator ci;
				ci = cm.find(ec);
				if (ci == cm.end()) {
					// Allocate new color
					cm[ec] = (*c);
					c++;
					if (c == color_names.end())
						c = color_names.begin();
					ci = cm.find(ec);
				}
				cout << "<font color=\"#" << (*ci).second << "\">";
				int len = ec->get_len();
				cout << (char)val;
				for (int j = 1; j < len; j++)
					cout << html((char)in.get());
				cout << "</font>";
				continue;
			}
			cout << html((char)val);
		}
	}
	cout << "</body></html>\n";
	return (0);
}
