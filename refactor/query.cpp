/*
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Encapsulates the common parts of a (user interface) query
 *
 * $Id: query.cpp,v 1.6 2005/05/15 14:03:51 dds Exp $
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
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <cassert>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi

#include <regex.h>
#include "swill.h"

#include "cpp.h"
#include "debug.h"
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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "license.h"
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "query.h"

bool Query::sort_rev;			// Reverse sort of identifier names

// URL-encode the given string
string
Query::url(const string &s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i == ' ')
			r += '+';
		else if (isalnum(*i) || *i == '_')
			r += *i;
		else {
			char buff[4];

			sprintf(buff, "%%%02x", (unsigned)*i);
			r += buff;
		}
	return r;
}

// Compile regular expression specs
bool
Query::compile_re(FILE *of, const char *name, const char *varname, regex_t &re, bool &match,  string &str, int compflags)
{
	char *s;
	int ret;
	match = false;
	if ((s = swill_getvar(varname)) && *s) {
		match = true;
		str = s;
		if ((ret = regcomp(&re, s, REG_EXTENDED | REG_NOSUB | compflags))) {
			char buff[1024];
			regerror(ret, &re, buff, sizeof(buff));
			fprintf(of, "<h2>%s regular expression error</h2>%s", name, buff);
			valid = return_val = false;
			lazy = true;
			return false;
		}
	}
	return true;
}

// Display an equality selection box
void
Query::equality_selection(FILE *of)
{
	fprintf(of,
		"<option value=\"%d\">ignore"
		"<option value=\"%d\">=="
		"<option value=\"%d\">!="
		"<option value=\"%d\">&lt;"
		"<option value=\"%d\">&gt;"
		"</select>",
		ec_ignore, ec_eq, ec_ne, ec_lt, ec_gt);
}
