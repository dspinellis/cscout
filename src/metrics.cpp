/*
 * (C) Copyright 2002-2024 Diomidis Spinellis
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
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
#include <cmath>		// log
#include <errno.h>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "filedetails.h"
#include "tokid.h"
#include "tokmap.h"
#include "eclass.h"
#include "fchar.h"
#include "token.h"
#include "ctoken.h"
#include "pltoken.h"
#include "call.h"

vector<bool> Metrics::is_operator_map(make_is_operator());
KeywordMetrics::map_type KeywordMetrics::map(make_keyword_map());

MetricDetails Metrics::metric_details[] = {
// BEGIN AUTOSCHEMA Metrics
	{ em_nchar,		"NCHAR",		"Number of characters"},
	{ em_nccomment,		"NCCOMMENT",		"Number of comment characters"},
	{ em_nspace,		"NSPACE",		"Number of space characters"},
	{ em_nlcomment,		"NLCOMMENT",		"Number of line comments"},
	{ em_nbcomment,		"NBCOMMENT",		"Number of block comments"},
	{ em_nline,		"NLINE",		"Number of lines"},
	{ em_maxlinelen,	"MAXLINELEN",		"Maximum number of characters in a line"},
	{ em_nstring,		"NSTRING",		"Number of character strings"},
	{ em_nuline,		"NULINE",		"Number of unprocessed lines"},

	{ em_npptoken,		"NPPTOKEN",		"Number of preprocessed tokens"},
	{ em_nctoken,		"NCTOKEN",		"Number of compiled tokens"},
	{ em_nppdirective,	"NPPDIRECTIVE",		"Number of C preprocessor directives"},
	{ em_nppcond,		"NPPCOND",		"Number of processed C preprocessor conditionals (ifdef, if, elif)"},
	{ em_nppfmacro,		"NPPFMACRO",		"Number of defined C preprocessor function-like macros"},
	{ em_nppomacro,		"NPPOMACRO",		"Number of defined C preprocessor object-like macros"},
	// Elements counted at the token tap
	{ em_nstmt,		"NSTMT",		"Number of statements or declarations"},
	{ em_nop,		"NOP",			"Number of operators"},
	{ em_nuop,		"NUOP",			"Number of unique operators"},
	{ em_nnconst,		"NNCONST",		"Number of numeric constants"},
	{ em_nclit,		"NCLIT",		"Number of character literals"},
	{ em_ncc2op,		"INTERNAL",		"Number of operators contributing to cc2: &&, ||, ?:"},
	// Keywords counted at the token tap
	{ em_nif,		"NIF",			"Number of if statements"},
	{ em_nelse,		"NELSE",		"Number of else clauses"},
	{ em_nswitch,		"NSWITCH",		"Number of switch statements"},
	{ em_ncase,		"NCASE",		"Number of case labels"},
	{ em_ndefault,		"NDEFAULT",		"Number of default labels"},
	{ em_nbreak,		"NBREAK",		"Number of break statements"},
	{ em_nfor,		"NFOR",			"Number of for statements"},
	{ em_nwhile,		"NWHILE",		"Number of while statements"},
	{ em_ndo,		"NDO",			"Number of do statements"},
	{ em_ncontinue,		"NCONTINUE",		"Number of continue statements"},
	{ em_ngoto,		"NGOTO",		"Number of goto statements"},
	{ em_nreturn,		"NRETURN",		"Number of return statements"},
	// Identifiers categorized during identifier processing
	{ em_npid,		"NPID",			"Number of project-scope identifiers"},
	{ em_nfid,		"NFID",			"Number of file-scope (static) identifiers"},
	{ em_nmid,		"NMID",			"Number of macro identifiers"},
	{ em_nid,		"NID",			"Total number of object and object-like identifiers"},
	{ em_nupid,		"NUPID",		"Number of unique project-scope identifiers"},
	{ em_nufid,		"NUFID",		"Number of unique file-scope (static) identifiers"},
	{ em_numid,		"NUMID",		"Number of unique macro identifiers"},
	{ em_nuid,		"NUID",			"Number of unique object and object-like identifiers"},
	{ em_nlabid,		"INTERNAL",		"Number of label identifiers"},
// END AUTOSCHEMA Metrics
	{ metric_max,		"",			""},
};

// Global metrics
IdMetricsSummary id_msum;

// Called for every identifier
void
Metrics::process_identifier(const string &s, Eclass *ec)
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
		else if (c == '\'')
			cstate = s_char;
		else if (c == '"') {
			cstate = s_string;
			count[em_nstring]++;
		}
		break;
	case s_char:
		if (c == '\'')
			cstate = s_normal;
		else if (c == '\\')
			cstate = s_saw_chr_backslash;
		break;
	case s_string:
		if (c == '"')
			cstate = s_normal;
		else if (c == '\\')
			cstate = s_saw_str_backslash;
		break;
	case s_saw_chr_backslash:
		cstate = s_char;
		break;
	case s_saw_str_backslash:
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

ostream&
operator<<(ostream& o, const IdMetricsSet &mi)
{
	IdMetricsSet &m = (IdMetricsSet &)mi;

	o << "<table class='metrics'>"
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
operator<<(ostream& o, const IdMetricsSummary &ms)
{
	o << "<h2>Writable Identifiers</h2>\n" << ms.rw[false];
	o << "<h2>Read-only Identifiers</h2>\n" << ms.rw[true];
	return o;
}

string
avg(double v, double n)
{
	if (n == 0.)
		return "-";
	ostringstream r;
	r << v / n;
	return r.str();
}

// Call the specified metrics function for the current file and function
// to tally metrics before the C preprocessor processing.
void
Metrics::call_pre_cpp_metrics(void (Metrics::*fun)())
{
	(Filedetails::get_pre_cpp_metrics(Fchar::get_fileid()).*fun)();
	Call::call_pre_cpp_metrics(fun);
}

// Call the specified metrics function for the current file and function
// to tally metrics after the C preprocessor processing.
void
Metrics::call_post_cpp_metrics(void (Metrics::*fun)())
{
	(Filedetails::get_post_cpp_metrics(Fchar::get_fileid()).*fun)();
	Call::call_post_cpp_metrics(fun);
}

// Initialize map
KeywordMetrics::map_type
KeywordMetrics::make_keyword_map()
{
	map_type km;

	km.insert(map_type::value_type("if", Metrics::em_nif));
	km.insert(map_type::value_type("else", Metrics::em_nelse));
	km.insert(map_type::value_type("switch", Metrics::em_nswitch));
	km.insert(map_type::value_type("case", Metrics::em_ncase));
	km.insert(map_type::value_type("default", Metrics::em_ndefault));
	km.insert(map_type::value_type("break", Metrics::em_nbreak));
	km.insert(map_type::value_type("for", Metrics::em_nfor));
	km.insert(map_type::value_type("while", Metrics::em_nwhile));
	km.insert(map_type::value_type("do", Metrics::em_ndo));
	km.insert(map_type::value_type("continue", Metrics::em_ncontinue));
	km.insert(map_type::value_type("goto", Metrics::em_ngoto));
	km.insert(map_type::value_type("return", Metrics::em_nreturn));

	return (km);
}


// Add operator op to int-indexed map v
inline void
Metrics::add_operator(vector<bool> &v, unsigned op)
{
	if (op >= v.size())
		v.resize(op + 1, false);
	v[op] = true;
}

// Create an integer-indexed map indicating which tokens represent operators
vector<bool>
Metrics::make_is_operator()
{
	vector<bool> isop;	// Tokens that are operators

	add_operator(isop, '!');
	add_operator(isop, '%');
	add_operator(isop, '&');
	add_operator(isop, '*');
	add_operator(isop, '+');
	// Comma is so overloaded as a token,
	// that it doesn't make sense to count it as an operator
	// add_operator(isop, ',');
	add_operator(isop, '-');
	add_operator(isop, '.');
	add_operator(isop, '/');
	add_operator(isop, '<');
	add_operator(isop, '=');
	add_operator(isop, '>');
	add_operator(isop, '?');
	add_operator(isop, '^');
	add_operator(isop, '|');
	add_operator(isop, '~');
	add_operator(isop, ADD_ASSIGN);
	add_operator(isop, AND_ASSIGN);
	add_operator(isop, AND_OP);
	add_operator(isop, CPP_CONCAT);
	add_operator(isop, DEC_OP);
	add_operator(isop, DIV_ASSIGN);
	add_operator(isop, EQ_OP);
	add_operator(isop, GE_OP);
	add_operator(isop, INC_OP);
	add_operator(isop, LEFT_ASSIGN);
	add_operator(isop, LEFT_OP);
	add_operator(isop, LE_OP);
	add_operator(isop, MOD_ASSIGN);
	add_operator(isop, MUL_ASSIGN);
	add_operator(isop, NE_OP);
	add_operator(isop, OR_ASSIGN);
	add_operator(isop, OR_OP);
	add_operator(isop, PTR_OP);
	add_operator(isop, RIGHT_ASSIGN);
	add_operator(isop, RIGHT_OP);
	add_operator(isop, SUB_ASSIGN);
	add_operator(isop, XOR_ASSIGN);
	return (isop);
}
