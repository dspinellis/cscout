/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: metrics.cpp,v 1.7 2003/05/27 17:46:33 dds Exp $
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

// Keep this in sync with the enumeration
string Metrics::metric_names[] = {
	"Number of characters",
	"Comment characters",
	"Space characters",
	"Number of line comments",
	"Number of block comments",
	"Number of lines",
	"Length of longest line",
	"Number of C strings",

	"Number of defined functions",
	"Number of preprocessor directives",
	"Number of directly included files",
	"Number of C statements",
};

// Global metrics
MetricsSummary msum;

// Called for every identifier
void 
Metrics::process_id(const string &s)
{
	count[em_nchar] += s.length();
	currlinelen += s.length();
}

// Called for all file characters appart from identifiers
void 
Metrics::process_char(char c)
{
	count[em_nchar]++;
	if (c == '\n') {
		if (DP())
			cout << "nline = " << count[em_nline] << "\n";
		count[em_nline]++;
		if (currlinelen > count[em_maxlinelen])
			count[em_maxlinelen] = currlinelen;
		currlinelen = 0;
	} else
		currlinelen++;
	switch (cstate) {
	case s_normal:
		if (isspace(c))
			count[em_nspace]++;
		else if (c == '/')
			cstate = s_saw_slash;
		else if (c == '"') {
			cstate = s_string;
			count[em_nstring]++;
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
			count[em_nlcomment]++;
		} else if (c == '*') {
			cstate = s_block_comment;
			count[em_nbcomment]++;
		} else
			cstate = s_normal;
		break;
	case s_cpp_comment:		// Inside C++ comment
		if (c == '\n')
			cstate = s_normal;
		else
			count[em_nccomment]++;
		break;
	case s_block_comment:		// Inside C block comment
		if (c == '*')
			cstate = s_block_star;
		count[em_nccomment]++;
		break;
	case s_block_star:		// Found a * in a block comment
		if (c == '/')
			cstate = s_normal;
		else
			cstate = s_block_comment;
		count[em_nccomment]--;		// Don't count the trailing */
		break;
	default:
		assert(0);
	}
}

// Adjust class members by n according to the attributes of EC
template <class UnaryFunction>
void
IdCount::add(Eclass *ec, UnaryFunction f)
{
	total = f(total);
	// The four C namespaces
	if (ec->get_attribute(is_suetag))
		suetag = f(suetag);
	if (ec->get_attribute(is_sumember))
		sumember = f(sumember);
	if (ec->get_attribute(is_label))
		label = f(label);
	if (ec->get_attribute(is_ordinary)) {
		ordinary = f(ordinary);
		if (ec->get_attribute(is_typedef))
			xtypedef = f(xtypedef);
		else {
			if (ec->get_attribute(is_cscope))
				cscope = f(cscope);
			if (ec->get_attribute(is_lscope))
				lscope = f(lscope);
		}
	}
	if (ec->get_attribute(is_macro))
		macro = f(macro);
	if (ec->get_attribute(is_macroarg))
		macroarg = f(macroarg);
	if (ec->get_size() == 1)
		unused = f(unused);
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

struct add_one : public unary_function<int, int>
{
      int operator()(int x) { return x + 1; }
};

struct add_n : public unary_function<int, int>
{
      int n;
      add_n(int add) { n = add; }
      int operator()(int x) { return x + n; }
};

struct set_max : public unary_function<int, int>
{
      int n;
      set_max(int newval) { n = newval; }
      int operator()(int x) { return x > n ? x : n; }
};

struct set_min : public unary_function<int, int>
{
      int n;
      set_min(int newval) { n = newval; }
      int operator()(int x) { return (x < n && x > 0) ? x : n; }
};

// Called for each identifier occurence (all)
void
MetricsSummary::add_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].all.add(ec, add_one());
}

// Called for each unique identifier occurence (EC)
void
MetricsSummary::add_unique_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].once.add(ec, add_one());
	rw[ec->get_attribute(is_readonly)].len.add(ec, add_n(ec->get_len()));
	rw[ec->get_attribute(is_readonly)].maxlen.add(ec, set_max(ec->get_len()));
	rw[ec->get_attribute(is_readonly)].minlen.add(ec, set_min(ec->get_len()));
}


ostream&
operator<<(ostream& o,const IdCount &i)
{
	o << 
		"total " << i.total << '\n' <<
		"suetag " << i.suetag << '\n' <<
		"sumember " << i.sumember << '\n' <<
		"label " << i.label << '\n' <<
		"ordinary " << i.ordinary << '\n' <<
		"macro " << i.macro << '\n' <<
		"macroarg " << i.macroarg << '\n' <<
		"cscope " << i.cscope << '\n' <<
		"lscope " << i.lscope << '\n' <<
		"typedef " << i.xtypedef << '\n' <<
		"unused " << i.unused << '\n';
	return o;
}

ostream&
operator<<(ostream& o,const FileCount &fc)
{
	o <<
		"nfile " << fc.nfile << '\n' <<
		"nchar " << fc.nchar << '\n' <<
		"nlcomment " << fc.nlcomment << '\n' <<
		"nbcomment " << fc.nbcomment << '\n' <<
		"nline " << fc.nline << '\n' <<
		"maxlinelen " << fc.maxlinelen << '\n' <<
		"nccomment " << fc.nccomment << '\n' <<
		"nspace " << fc.nspace << '\n' <<
		"nstring " << fc.nstring << '\n' <<
		"nfunction " << fc.nfunction << '\n' <<
		"nppdirective " << fc.nppdirective << '\n' <<
		"nincfile " << fc.nincfile << '\n' <<
		"nstatement " << fc.nstatement << '\n';
	return o;
}

ostream&
operator<<(ostream& o,const MetricsSet &m)
{
	o << 
		"file\n" << m.fc <<
		"once\n" << m.once <<
		"len\n" << m.len <<
		"maxlen\n" << m.maxlen <<
		"minlen\n" << m.minlen <<
		"all\n" << m.all;
	return o;
}

ostream&
operator<<(ostream& o,const MetricsSummary &ms)
{
	o << "Writable\n" << ms.rw[false];
	o << "Read-only\n" << ms.rw[true];
	return o;
}
