/*
 * (C) Copyright 2011 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Create a ctags-compatible lookup file
 *
 * $Id: ctag.cpp,v 1.1 2011/10/23 16:22:34 dds Exp $
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

#include "ytab.h"

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "ptoken.h"
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
			i->definition.get_fileid().line_number(i->definition.get_streampos()) << '\t' <<// Line number
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
			if (i->definition.get_fileid().get_includers().size() == 0)
				out << "\tfile:";
			break;
		}

		out << endl;
	}
}
