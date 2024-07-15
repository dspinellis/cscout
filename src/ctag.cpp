/*
 * (C) Copyright 2011-2024 Diomidis Spinellis
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
 *
 * Create a ctags-compatible lookup file
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <fstream>
#include <cstdio>

#include "parse.tab.h"

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "filedetails.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "ptoken.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "ctag.h"
#include "version.h"

set<CTag> CTag::ctags;
bool CTag::enabled;

/*
 * C kinds supported by ctags
 *   d  macro definitions
 *   e  enumerators (values inside an enumeration)
 *   f  function definitions
 *   g  enumeration names
 *   m  class, struct, and union members
 *   s  structure names
 *   t  typedefs
 *   u  union names
 *   v  variable definitions
 *
 * Not enabled by default in ctags, not supported by CSCout
 *   l  local variables [off]
 *   p  function prototypes [off]
 *   x  external and forward variable declarations [off]
*/
void
CTag::save()
{
	if (!enabled)
		return;

	ofstream out;
	out.open("tags");
	if (out.fail()) {
		perror("tags");
		exit(1);
	}

	// Header
	out << "!_TAG_FILE_FORMAT	2	/extended format; --format=1 will not append ;\" to lines/" << endl;
	out << "!_TAG_FILE_SORTED	1	/0=unsorted, 1=sorted, 2=foldcase/" << endl;
	out << "!_TAG_PROGRAM_AUTHOR	Diomidis Spinellis	/dds@spinellis.gr/" << endl;
	out << "!_TAG_PROGRAM_NAME	CScout	//" << endl;
	out << "!_TAG_PROGRAM_URL	http://http://www.spinellis.gr/cscout/	/official site/" << endl;
	out << "!_TAG_PROGRAM_VERSION	" << Version::get_revision() << "	//" << endl;

	// The actual tags
	for (set<CTag>::const_iterator i = ctags.begin(); i != ctags.end(); i++) {
		out <<
			i->name << '\t' <<						// Identifier
			i->definition.get_path() << '\t' <<				// File
			Filedetails::get_line_number(i->definition.get_fileid(), i->definition.get_streampos()) << '\t' <<// Line number
			"\t;\"";							// Extended information

		/*
		 * When tag is non-empty use kind to emit it as struct/union/enum and
		 * emit type as m
		 */
		if (i->tag.size()) {
			out << "\tm";
			switch (i->kind) {
			case 's':
				out << "\tstruct:";
				break;
			case 'u':
				out << "\tunion:";
				break;
			case 'e':
				out << "\tenum:";
				break;
			}
			out << i->tag;
		} else
			out << '\t' << i->kind;

		// For all but f, v kinds, if the file is not included emit file:
		switch (i->kind) {
		case 'f':
		case 'v':
			if (i->is_static)
				out << "\tfile:";
			break;
		default:
			if (Filedetails::get_includers(i->definition.get_fileid()).size() == 0)
				out << "\tfile:";
			break;
		}

		out << endl;
	}
}
