/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * Encapsulates a (user interface) function query
 *
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
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "compiledre.h"
#include "option.h"
#include "query.h"
#include "mquery.h"
#include "funquery.h"

int FunQuery::specified_order::order;
bool FunQuery::specified_order::reverse;

// Construct an object based on URL parameters
FunQuery::FunQuery(FILE *of, bool icase, Attributes::size_type cp, bool e, bool r) :
	Query(!e, r, true),
	match_fid(false),
	id_ec(NULL),
	call(NULL),
	current_project(cp)
{
	if (lazy)
		return;

	valid = true;

	// Query name
	char *qname = swill_getvar("n");
	if (qname && *qname)
		name = qname;

	// Match specific file
	int ifid;
	if (swill_getargs("i(fid)", &ifid)) {
		match_fid = true;
		fid = Fileid(ifid);
	}

	// Function call declaration direct match
	if (!swill_getargs("p(call)", &call))
		call = NULL;

	// Identifier EC match
	if (!swill_getargs("p(ec)", &id_ec)) {
		id_ec = NULL;

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
	mquery.set_match_type(match_type);

	cfun = !!swill_getvar("cfun");
	macro = !!swill_getvar("macro");
	writable = !!swill_getvar("writable");
	ro = !!swill_getvar("ro");
	pscope = !!swill_getvar("pscope");
	fscope = !!swill_getvar("fscope");
	defined = !!swill_getvar("defined");
	if (!swill_getargs("i(ncallers)|i(ncallerop)", &ncallers, &ncallerop))
		ncallerop = ec_ignore;

	exclude_fnre = !!swill_getvar("xfnre");
	exclude_fure = !!swill_getvar("xfure");
	exclude_fdre = !!swill_getvar("xfdre");
	exclude_fre = !!swill_getvar("xfre");

	// Compile regular expression specs
	if (!compile_re(of, "Function name", "fnre", fnre, match_fnre, str_fnre) ||
	    !compile_re(of, "Calling function name", "fure", fure, match_fure, str_fure) ||
	    !compile_re(of, "Called function name", "fdre", fdre, match_fdre, str_fdre) ||
	    !compile_re(of, "Filename", "fre", fre, match_fre, str_fre, (icase ? REG_ICASE : 0)))
	    	return;
	specified_order::set_order(mquery.get_sort_order(), mquery.get_reverse());
}

// Return the URL for re-executing this query
string
FunQuery::base_url() const
{
	return string("xfunquery.html?") + param_url();
}

// Return the query's parameters as a URL
string
FunQuery::param_url() const
{
	char buff[256];

	string r("qt=fun");
	if (id_ec) {
		char buff[256];

		sprintf(buff, "&ec=%p", id_ec);
		r += buff;
	} else {
		r += "&match=";
		r += Query::url(string(1, match_type));
	}
	r += mquery.param_url();
	if (call) {
		sprintf(buff, "&call=%p", call);
		r += buff;
	}
	if (match_fid) {
		sprintf(buff, "&fid=%d", fid.get_id());
		r += buff;
	}
	if (cfun)
		r += "&cfun=1";
	if (macro)
		r += "&macro=1";
	if (writable)
		r += "&writable=1";
	if (ro)
		r += "&ro=1";
	if (pscope)
		r += "&pscope=1";
	if (fscope)
		r += "&fscope=1";
	if (defined)
		r += "&defined=1";
	if (match_fnre)
		r += "&fnre=" + Query::url(str_fnre);
	if (match_fure)
		r += "&fure=" + Query::url(str_fure);
	if (match_fdre)
		r += "&fdre=" + Query::url(str_fdre);
	if (match_fre)
		r += "&fre=" + Query::url(str_fre);
	if (exclude_fnre)
		r += "&xfnre=1";
	if (exclude_fure)
		r += "&xfure=1";
	if (exclude_fdre)
		r += "&xfdre=1";
	if (exclude_fre)
		r += "&xfre=1";
	if (ncallerop != ec_ignore) {
		ostringstream varname;

		varname << "&ncallers=" << ncallers;
		varname << "&ncallerop=" << ncallerop;
		r += varname.str();
	} else
		r += "&ncallerop=0";
	if (name.length())
		r += "&n=" + Query::url(name);
	return r;
}

// Evaluate the object's identifier query against i
// return true if it matches
bool
FunQuery::eval(Call *c)
{
	if (lazy)
		return return_val;

	if (call)
		return (c == call);

	if (id_ec) {
		if (!c->is_span_valid())
			return false;
		const setTokid &m = id_ec->get_members();
		for (setTokid::const_iterator i = m.begin(); i != m.end(); i++)
			if (*i >= c->get_begin().get_tokid() && *i <= c->get_end().get_tokid())
			    	return true;
		return false;
	}

	if (match_fid && c->get_begin().get_tokid().get_fileid() != fid)
		return false;

	Eclass *ec = c->get_tokid().get_ec();
	if (current_project && !ec->get_attribute(current_project))
		return false;

	bool add = mquery.eval(*c);
	switch (match_type) {
	case 'Y':	// anY match
		add = (add || (cfun && c->is_cfun()));
		add = (add || (macro && c->is_macro()));
		add = (add || (writable && !ec->get_attribute(is_readonly)));
		add = (add || (ro && ec->get_attribute(is_readonly)));
		add = (add || (pscope && !c->is_file_scoped()));
		add = (add || (fscope && c->is_file_scoped()));
		add = (add || (defined && c->is_defined()));
		break;
	case 'L':	// alL match
		add = (add && (!cfun || c->is_cfun()));
		add = (add && (!macro || c->is_macro()));
		add = (add && (!writable || !ec->get_attribute(is_readonly)));
		add = (add && (!ro || ec->get_attribute(is_readonly)));
		add = (add && (!pscope || !c->is_file_scoped()));
		add = (add && (!fscope || c->is_file_scoped()));
		add = (add && (!defined || c->is_defined()));
		break;
	case 'E':	// excludE match
		add = (add && (!cfun || !c->is_cfun()));
		add = (add && (!macro || !c->is_macro()));
		add = (add && (!writable || ec->get_attribute(is_readonly)));
		add = (add && (!ro || !ec->get_attribute(is_readonly)));
		add = (add && (!pscope || c->is_file_scoped()));
		add = (add && (!fscope || !c->is_file_scoped()));
		add = (add && (!defined || !c->is_defined()));
		break;
	case 'T':	// exactT match
		add = (add && (cfun == c->is_cfun()));
		add = (add && (macro == c->is_macro()));
		add = (add && (writable == !ec->get_attribute(is_readonly)));
		add = (add && (ro == ec->get_attribute(is_readonly)));
		add = (add && (pscope == !c->is_file_scoped()));
		add = (add && (fscope == c->is_file_scoped()));
		add = (add && (defined == c->is_defined()));
		break;
	}
	if (!add)
		return false;

	if (ncallerop && !Query::apply(ncallerop, c->get_num_caller(), ncallers))
		return false;

	int retval = exclude_fnre ? 0 : REG_NOMATCH;
	if (match_fnre && fnre.exec(c->get_name()) == retval)
		return false;

	retval = exclude_fre ? 0 : REG_NOMATCH;
	if (match_fre && fre.exec(c->get_fileid().get_path()) == retval)
			return false;

	Call::const_fiterator_type c2;
	if (match_fdre) {
		for (c2 = c->call_begin(); c2 != c->call_end(); c2++)
			if (fdre.exec((*c2)->get_name()) == 0) {
				if (exclude_fdre)
					return false;
				else
					break;
			}
		if (!exclude_fdre && c2 == c->call_end())
			return false;
	}
	if (match_fure) {
		for (c2 = c->caller_begin(); c2 != c->caller_end(); c2++)
			if (fure.exec((*c2)->get_name()) == 0) {
				if (exclude_fure)
					return false;
				else
					break;
			}
		if (!exclude_fure && c2 == c->caller_end())
			return false;
	}
	return true;
}
