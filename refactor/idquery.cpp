/*
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Encapsulates an (user interface) identifier query
 * Can be used to evaluate against IdProp elements
 *
 * $Id: idquery.cpp,v 1.7 2005/05/15 14:03:51 dds Exp $
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
#include "getopt.h"

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
#include "idquery.h"

// Construct an object based on URL parameters
IdQuery::IdQuery(FILE *of, bool icase, Attributes::size_type cp, bool e, bool r) :
	Query(!e, r, true),
	match(attr_end),
	current_project(cp)
{
	if (lazy)
		return;
	// Query name
	char *qname = swill_getvar("n");
	if (qname && *qname)
		name = qname;

	// Identifier EC match
	if (!swill_getargs("p(ec)", &ec)) {
		ec = NULL;

		// Type of boolean match
		char *m;
		if (!(m = swill_getvar("match"))) {
			fprintf(of, "Missing value: match");
			valid = return_val = false;
			lazy = true;
			return;
		}
		match_type = *m;
	}

	xfile = !!swill_getvar("xfile");
	unused = !!swill_getvar("unused");
	writable = !!swill_getvar("writable");
	exclude_ire = !!swill_getvar("xire");

	// Compile regular expression specs
	if (!compile_re(of, "Identifier", "ire", ire, match_ire, str_ire))
		return;
	if (!compile_re(of, "Filename", "fnre", fre, match_fre, str_fre, (icase ? REG_ICASE : 0)))
		return;

	// Store match specifications in a vector
	for (int i = attr_begin; i < attr_end; i++) {
		ostringstream varname;

		varname << "a" << i;
		match[i] = !!swill_getvar(varname.str().c_str());
		if (DP())
			cout << "v=[" << varname.str() << "] m=" << match[i] << "\n";
	}
}

// Report the string query specification usage
void
IdQuery::usage(void)
{
	cerr << "The monitored identifier attributes must be specified using the syntax:\n"
		"Y|L|E|T[:attr1][:attr2]...\n"
		"\tY: Match any of the specified attributes\n"
		"\tL: Match all of the specified attributes\n"
		"\tE: Exclude the specified attributes matched\n"
		"\tT: Exact match of the specified attributes\n\n"

		"Allowable attribute names are:\n"
		"\tunused: Unused\n"
		"\twritable: Writable\n";
	for (int i = attr_begin; i < attr_end; i++)
		cerr << "\t" << Attributes::shortname(i) << ": " << Attributes::name(i) << "\n";
	exit(1);
}

// Construct an object based on a string specification
// The syntax is Y|L|E|T[:attr1][:attr2]...
IdQuery::IdQuery(const string &s) :
	Query(false, false, true),
	match_fre(false),
	match_ire(false),
	match(attr_end),
	xfile(false),
	ec(NULL)
{
	// Type of boolean match
	if (s.length() == 0)
		usage();
	switch (s[0]) {
	case 'Y':
	case 'L':
	case 'E':
	case 'T':
		match_type = s[0];
		break;
	default:
		usage();
	}

	unused = (s.find(":unused") != string::npos);
	writable = (s.find(":writable") != string::npos);

	// Store match specifications in a vector
	for (int i = attr_begin; i < attr_end; i++)
		match[i] = (s.find(":" + Attributes::shortname(i)) != string::npos);
}

// Return the query's parameters as a URL
string
IdQuery::url() const
{
	string r("qt=id&match=");
	r += Query::url(string(1, match_type));
	if (ec) {
		char buff[256];

		sprintf(buff, "&ec=%p", ec);
		r += buff;
	}
	if (xfile)
		r += "&xfile=1";
	if (unused)
		r += "&unused=1";
	if (writable)
		r += "&writable=1";
	if (match_ire)
		r += "&ire=" + Query::url(str_ire);
	if (exclude_ire)
		r += "&xire=1";
	if (match_fre)
		r += "&fre=" + Query::url(str_fre);
	for (int i = attr_begin; i < attr_end; i++) {
		if (match[i]) {
			ostringstream varname;

			varname << "&a" << i << "=1";
			r += varname.str();
		}
	}
	if (name.length())
		r += "&n=" + Query::url(name);
	return r;
}

// Evaluate the object's identifier query against i
// return true if it matches
bool
IdQuery::eval(const IdPropElem &i)
{
	if (lazy)
		return return_val;

	if (ec)
		return (i.first == ec);
	if (current_project && !i.first->get_attribute(current_project))
		return false;
	int retval = exclude_ire ? 0 : REG_NOMATCH;
	if (match_ire && regexec(&ire, i.second.get_id().c_str(), 0, NULL, 0) == retval)
		return false;
	bool add;
	switch (match_type) {
	case 'Y':	// anY match
		add = false;
		for (int j = attr_begin; j < attr_end; j++)
			if (match[j] && i.first->get_attribute(j)) {
				add = true;
				break;
			}
		add = (add || (xfile && i.second.get_xfile()));
		add = (add || (unused && i.first->is_unused()));
		add = (add || (writable && !i.first->get_attribute(is_readonly)));
		break;
	case 'L':	// alL match
		add = true;
		for (int j = attr_begin; j < attr_end; j++)
			if (match[j] && !i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (!xfile || i.second.get_xfile()));
		add = (add && (!unused || i.first->is_unused()));
		add = (add && (!writable || !i.first->get_attribute(is_readonly)));
		break;
	case 'E':	// excludE match
		add = true;
		for (int j = attr_begin; j < attr_end; j++)
			if (match[j] && i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (!xfile || !i.second.get_xfile()));
		add = (add && (!unused || !(i.first->is_unused())));
		add = (add && (!writable || i.first->get_attribute(is_readonly)));
		break;
	case 'T':	// exactT match
		add = true;
		for (int j = attr_begin; j < attr_end; j++)
			if (match[j] != i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (xfile == i.second.get_xfile()));
		add = (add && (unused == (i.first->is_unused())));
		add = (add && (writable == !i.first->get_attribute(is_readonly)));
		break;
	}
	if (!add)
		return false;
	if (match_fre) {
		// Before we add it check if its filename matches the RE
		IFSet f = i.first->sorted_files();
		IFSet::iterator j;
		for (j = f.begin(); j != f.end(); j++)
			if (regexec(&fre, (*j).get_path().c_str(), 0, NULL, 0) == 0)
				break;	// Yes is matches
		if (j == f.end())
			return false;	// No match found
	}
	return true;
}
