/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: metrics.cpp,v 1.4 2002/10/06 21:21:42 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
#include <list>
#include <errno.h>

#include "cpp.h"
#include "debug.h"
#include "metrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "tokmap.h"
#include "eclass.h"
#include "fchar.h"
#include "error.h"

// Called for every identifier
void 
Metrics::process_id(const string &s)
{
	nchar += s.length();
	currlinelen += s.length();
}

// Called for all file characters appart from identifiers
void 
Metrics::process_char(char c)
{
	nchar++;
	if (c == '\n') {
		if (DP())
			cout << "nline = " << nline << "\n";
		nline++;
		if (currlinelen > maxlinelen)
			maxlinelen = currlinelen;
		currlinelen = 0;
	} else
		currlinelen++;
	switch (cstate) {
	case s_normal:
		if (isspace(c))
			nspace++;
		else if (c == '/')
			cstate = s_saw_slash;
		else if (c == '"') {
			cstate = s_string;
			nstring++;
		}
		break;
	case s_string:
		if (c == '"')
			cstate = s_normal;
		else if (c == '\\')
			cstate = s_saw_backslash;
		break;
	case s_saw_backslash:
		cstate = s_string;
		break;
	case s_saw_slash:		// After a / character
		if (c == '/') {
			cstate = s_cpp_comment;
			nlcomment++;
		} else if (c == '*') {
			cstate = s_block_comment;
			nbcomment++;
		} else
			cstate = s_normal;
		break;
	case s_cpp_comment:		// Inside C++ comment
		if (c == '\n')
			cstate = s_normal;
		else
			nccomment++;
		break;
	case s_block_comment:		// Inside C block comment
		if (c == '*')
			cstate = s_block_star;
		nccomment++;
		break;
	case s_block_star:		// Found a * in a block comment
		if (c == '/')
			cstate = s_normal;
		else
			cstate = s_block_comment;
		nccomment--;		// Don't count the trailing */
		break;
	default:
		assert(0);
	}
}

// Adjust class members by n according to the attributes of EC
void
IdCount::add(Eclass *ec, int n)
{
	total += n;
	// The four C namespaces
	if (ec->get_attribute(is_suetag))
		suetag += n;
	if (ec->get_attribute(is_sumember))
		sumember += n;
	if (ec->get_attribute(is_label))
		label += n;
	if (ec->get_attribute(is_ordinary)) {
		ordinary += n;
		if (ec->get_attribute(is_cscope))
			cscope += n;
		if (ec->get_attribute(is_lscope))
			lscope += n;
	}
	if (ec->get_attribute(is_macro))
		macro += n;
	if (ec->get_attribute(is_macroarg))
		macroarg += n;
	if (ec->get_attribute(is_typedef))
		xtypedef += n;
	if (ec->get_size() == 1)
		unused += n;
}

// Update file-based summary
void
FileCount::add(Fileid &fi)
{
	nfile++;
	nchar += fi.metrics().get_nchar();
	nlcomment += fi.metrics().get_nlcomment();
	nbcomment += fi.metrics().get_nbcomment();
	nline += fi.metrics().get_nline();
	if (fi.metrics().get_maxlinelen() > maxlinelen)
		maxlinelen = fi.metrics().get_maxlinelen();
	nccomment += fi.metrics().get_nccomment();
	nspace += fi.metrics().get_nspace();
	nstring += fi.metrics().get_nstring();
	nfunction += fi.metrics().get_nfunction();
	nppdirective += fi.metrics().get_nppdirective();
	nincfile += fi.metrics().get_nincfile();
	nstatement += fi.metrics().get_nstatement();
}

// Create file-based summary
void
MetricsSummary::summarize_files()
{
	vector <Fileid> files = Fileid::sorted_files();
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++)
		rw[(*i).get_readonly()].fc.add(*i);
}

// Called for each identifier occurence (all)
void
MetricsSummary::add_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].all.add(ec, 1);
}

// Called for each unique identifier occurence (EC)
void
MetricsSummary::add_unique_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].once.add(ec, 1);
	rw[ec->get_attribute(is_readonly)].len.add(ec, ec->get_len());
}

// Global metrics
MetricsSummary msum;
