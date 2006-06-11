/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: metrics.cpp,v 1.17 2006/06/11 21:44:18 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
#include <sstream>		// ostringstream
#include <list>
#include <climits>		// INT_MAX
//#include <limits>		// numeric_limits
#include <errno.h>

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
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
	"Number of unprocessed lines",

	"Number of defined functions",
	"Number of preprocessor directives",
	"Number of directly included files",
	"Number of C statements",
};

// Global metrics
IdMetricsSummary id_msum;
FileMetricsSummary file_msum;

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
		if (c == '/') {
			cstate = s_normal;
			count[em_nccomment] -= 2;	// Don't count the trailing */
		} else if (c != '*')
			cstate = s_block_comment;
		count[em_nccomment]++;
		break;
	default:
		csassert(0);
	}
}

// Adjust class members by n according to the attributes of EC
template <class UnaryFunction>
void
IdCount::add(Eclass *ec, UnaryFunction f)
{
	total = f(total);
	// This counts typedefs as being file scope
	for (int i = attr_begin; i < attr_end; i++)
		if (ec->get_attribute(i))
			count[i] = f(count[i]);
}

// Update file-based summary
template <class BinaryFunction>
void
FileCount::add(Fileid &fi, BinaryFunction f)
{
	nfile++;
	for (int i = 0; i < metric_max; i++)
		count[i] = f(fi.metrics().get_metric(i), count[i]);
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

struct get_max : public binary_function<int, int, int>
{
      int operator()(int x, int y) { return (x < y) ? y : x; }
};

struct get_min : public binary_function<int, int, int>
{
      int operator()(int x, int y) { return (x > y) ? y : x; }
};

// Called for each identifier occurence (all)
void
IdMetricsSummary::add_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].all.add(ec, add_one());
}

// Called for each unique identifier occurence (EC)
void
IdMetricsSummary::add_unique_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].once.add(ec, add_one());
	rw[ec->get_attribute(is_readonly)].len.add(ec, add_n(ec->get_len()));
	rw[ec->get_attribute(is_readonly)].maxlen.add(ec, set_max(ec->get_len()));
	rw[ec->get_attribute(is_readonly)].minlen.add(ec, set_min(ec->get_len()));
}

// Create file-based summary
void
FileMetricsSummary::summarize_files()
{
	vector <Fileid> files = Fileid::files(false);
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		rw[(*i).get_attribute(is_readonly)].total.add((*i), plus<int>());
		rw[(*i).get_attribute(is_readonly)].min.add((*i), get_min());
		rw[(*i).get_attribute(is_readonly)].max.add((*i), get_max());
	}
}

FileMetricsSet::FileMetricsSet() :
	min(INT_MAX)
// When my Linux upgrades from gcc 2.96
//	min(numeric_limits<int>::max())
{
}

static string
avg(int v, int n)
{
	if (!n)
		return "-";
	ostringstream r;
	r << v / n;
	return r.str();
}

ostream&
operator<<(ostream& o, const FileMetricsSet &mi)
{
	FileMetricsSet &m = (FileMetricsSet &)mi;

	o << "Number of files: " << m.total.nfile << "<p>\n";
	if (m.total.nfile == 0)
		return o;
	o << "<table border=1>"
		"<tr><th>" "File metric" "</th>"
		"<th>" "Total" "</th>"
		"<th>" "Min" "</th>"
		"<th>" "Max" "</th>"
		"<th>" "Avg" "</th></tr>\n";
	for (int i = 0; i < metric_max; i++)
		o << "<tr><td>" << Metrics::name(i) << "</td>"
			"<td>" << m.total.get_metric(i) << "</td>"
			"<td>" << m.min.get_metric(i) << "</td>"
			"<td>" << m.max.get_metric(i) << "</td>"
			"<td>" << avg(m.total.get_metric(i), m.total.nfile) << "</td></tr>\n";
	o << "</table>\n";
	return o;
}

ostream&
operator<<(ostream& o, const IdMetricsSet &mi)
{
	IdMetricsSet &m = (IdMetricsSet &)mi;

	o << "<table border=1>"
		"<tr><th>" "Identifier class" "</th>"
		"<th>" "Distinct # ids" "</th>"
		"<th>" "Total # ids" "</th>"
		"<th>" "Avg length" "</th>"
		"<th>" "Min length" "</th>"
		"<th>" "Max length" "</th></tr>\n";
	o << "<tr><td>" "All identifiers" "</td>"
		"<td>" << m.once.total << "</td>"
		"<td>" << m.all.total << "</td>"
		"<td>" << avg(m.len.total, m.once.total) << "</td>"
		"<td>" << m.minlen.total << "</td>"
		"<td>" << m.maxlen.total << "</td></tr>\n";
	for (int i = is_readonly + 1; i < attr_end; i++)
		o << "<tr><td>" << Attributes::name(i) << "</td>"
			"<td>" << m.once.get_count(i) << "</td>"
			"<td>" << m.all.get_count(i) << "</td>"
			"<td>" << avg(m.len.get_count(i), m.once.get_count(i)) << "</td>"
			"<td>" << m.minlen.get_count(i) << "</td>"
			"<td>" << m.maxlen.get_count(i) << "</td></tr>\n";
	o << "</table>\n";
	return o;
}

ostream&
operator<<(ostream& o, const FileMetricsSummary &ms)
{
	o << "<h2>Writable Files</h2>\n" << ms.rw[false];
	o << "<h2>Read-only Files</h2>\n" << ms.rw[true];
	return o;
}

ostream&
operator<<(ostream& o, const IdMetricsSummary &ms)
{
	o << "<h2>Writable Identifiers</h2>\n" << ms.rw[false];
	o << "<h2>Read-only Identifiers</h2>\n" << ms.rw[true];
	return o;
}
