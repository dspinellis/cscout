/*
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Encapsulates a (user interface) file query
 *
 * $Id: filequery.cpp,v 1.1 2007/08/10 10:15:05 dds Exp $
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
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi

#include <regex.h>
#include "swill.h"
#include "getopt.h"

#include "cpp.h"
#include "debug.h"
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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "license.h"
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "compiledre.h"
#include "option.h"
#include "query.h"
#include "mquery.h"
#include "filequery.h"


int FileQuery::specified_order::order;
bool FileQuery::specified_order::reverse;

// Construct an object based on URL parameters
FileQuery::FileQuery(FILE *of, bool icase, Attributes::size_type cp, bool e, bool r) :
	Query(!e, r, true),
	current_project(cp)
{
	if (lazy)
		return;

	valid = true;

	// Query name
	char *qname = swill_getvar("n");
	if (qname && *qname)
		name = qname;

	// Type of boolean match
	char *m;
	if (!(m = swill_getvar("match"))) {
		fprintf(of, "Missing value: match");
		valid = return_val = false;
		lazy = true;
		return;
	}
	match_type = *m;
	mquery.set_match_type(match_type);

	writable = !!swill_getvar("writable");
	ro = !!swill_getvar("ro");

	// Compile regular expression specs
	if (!compile_re(of, "Filename", "fre", fre, match_fre, str_fre, (icase ? REG_ICASE : 0)))
	    	return;
	specified_order::set_order(mquery.get_sort_order(), mquery.get_reverse());
}

// Return the URL for re-executing this query
string
FileQuery::base_url() const
{
	return string("xfilequery.html?") + param_url();
}

// Return the query's parameters as a URL
string
FileQuery::param_url() const
{
	ostringstream url;
	url << "match=";
	url << Query::url(string(1, match_type));
	url << mquery.param_url();
	if (writable)
		url << "&writable=1";
	if (ro)
		url << "&ro=1";
	if (match_fre)
		url << "&fre=" << Query::url(str_fre);
	if (name.length())
		url << "&n=" << Query::url(name);
	return url.str();
}

// Evaluate the object's identifier query against i
// return true if it matches
bool
FileQuery::eval(Fileid &f)
{
	if (lazy)
		return return_val;

	if (current_project && !f.get_attribute(current_project))
		return false;

	bool add = mquery.eval(f);
	switch (match_type) {
	case 'Y':	// anY match
		add = (add || (ro && f.get_readonly()));
		add = (add || (writable && !f.get_readonly()));
		break;
	case 'L':	// alL match
		add = (add && (!ro || f.get_readonly()));
		add = (add && (!writable || !f.get_readonly()));
		break;
	}
	if (!add)
		return false;

	if (match_fre && fre.exec(f.get_path()) == REG_NOMATCH)
			return false;	// No match found
	return true;
}
