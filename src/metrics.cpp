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
KeywordMetrics::map_type KeywordMetrics::keyword_map(make_keyword_map());

vector<MetricDetails>
Metrics::metric_details_values()
{
	vector<MetricDetails> v(metric_max);
						// pre-cpp, post-cpp, file
	v[em_nchar]		= MetricDetails(1, 0, 1, "NCHAR",	"Number of characters");
	v[em_nccomment]		= MetricDetails(1, 0, 1, "NCCOMMENT",	"Number of comment characters");
	v[em_nspace]		= MetricDetails(1, 0, 1, "NSPACE",	"Number of space characters");
	v[em_nlcomment]		= MetricDetails(1, 0, 1, "NLCOMMENT",	"Number of line comments");
	v[em_nbcomment]		= MetricDetails(1, 0, 1, "NBCOMMENT",	"Number of block comments");
	v[em_nline]		= MetricDetails(1, 0, 1, "NLINE",	"Number of lines");
	v[em_maxlinelen]	= MetricDetails(1, 0, 1, "MAXLINELEN",	"Maximum number of characters in a line");
	v[em_maxstmtlen]	= MetricDetails(1, 1, 1, "MAXSTMTLEN",	"Maximum number of tokens in a statement");
	v[em_maxstmtnest]	= MetricDetails(0, 1, 1, "MAXSTMTNEST",	"Maximum level of statement nesting");
	v[em_maxbracenest]	= MetricDetails(1, 1, 1, "MAXBRACENEST","Maximum level of brace nesting");
	v[em_maxbracknest]	= MetricDetails(1, 1, 1, "MAXBRACKNEST","Maximum level of bracket nesting");
	v[em_nuline]		= MetricDetails(1, 0, 1, "NULINE",	"Number of unprocessed lines");

	v[em_ntoken]		= MetricDetails(1, 1, 1, "NTOKEN",	"Number of tokens");
	v[em_nppdirective]	= MetricDetails(1, 0, 1, "NPPDIRECTIVE","Number of C preprocessor directives");
	v[em_nppcond]		= MetricDetails(1, 0, 1, "NPPCOND",	"Number of processed C preprocessor conditionals (ifdef, if, elif)");
	v[em_nppfmacro]		= MetricDetails(1, 0, 1, "NPPFMACRO",	"Number of defined C preprocessor function-like macros");
	v[em_nppomacro]		= MetricDetails(1, 0, 1, "NPPOMACRO",	"Number of defined C preprocessor object-like macros");
	v[em_nconcatop]		= MetricDetails(1, 0, 1, "NPPCONCATOP",	"Number of token concatenation operators (##)");
	v[em_nstringop]		= MetricDetails(1, 0, 1, "NPPSTRINGOP",	"Number of token stringification operators (#)");
	// Elements counted at the token tap
	v[em_nstmt]		= MetricDetails(1, 1, 1, "NSTMT",	"Number of statements or declarations");
	v[em_nop]		= MetricDetails(1, 1, 1, "NOP",		"Number of operators");
	v[em_nuop]		= MetricDetails(1, 1, 0, "NUOP",	"Number of unique operators");
	v[em_nnconst]		= MetricDetails(1, 1, 1, "NNCONST",	"Number of numeric constants");
	v[em_nclit]		= MetricDetails(1, 1, 1, "NCLIT",	"Number of character literals");
	v[em_nstring]		= MetricDetails(1, 1, 1, "NSTRING",	"Number of character strings");
	v[em_ncc2op]		= MetricDetails(0, 0, 0, "INTERNAL",	"Number of operators contributing to cc2: &&, ||, ?:");
	// Keywords counted at the token tap
	v[em_nif]		= MetricDetails(1, 1, 1, "NIF",		"Number of if statements");
	v[em_nelse]		= MetricDetails(1, 1, 1, "NELSE",	"Number of else clauses");
	v[em_nswitch]		= MetricDetails(1, 1, 1, "NSWITCH",	"Number of switch statements");
	v[em_ncase]		= MetricDetails(1, 1, 1, "NCASE",	"Number of case labels");
	v[em_ndefault]		= MetricDetails(1, 1, 1, "NDEFAULT",	"Number of default labels");
	v[em_nbreak]		= MetricDetails(1, 1, 1, "NBREAK",	"Number of break statements");
	v[em_nfor]		= MetricDetails(1, 1, 1, "NFOR",	"Number of for statements");
	v[em_nwhile]		= MetricDetails(1, 1, 1, "NWHILE",	"Number of while statements");
	v[em_ndo]		= MetricDetails(1, 1, 1, "NDO",		"Number of do statements");
	v[em_ncontinue]		= MetricDetails(1, 1, 1, "NCONTINUE",	"Number of continue statements");
	v[em_ngoto]		= MetricDetails(1, 1, 1, "NGOTO",	"Number of goto statements");
	v[em_nreturn]		= MetricDetails(1, 1, 1, "NRETURN",	"Number of return statements");
	// Identifiers categorized during identifier processing
	v[em_npid]		= MetricDetails(1, 1, 1, "NPID",	"Number of project-scope identifiers");
	v[em_nfid]		= MetricDetails(1, 1, 1, "NFID",	"Number of file-scope (static) identifiers");
	v[em_nmid]		= MetricDetails(1, 0, 1, "NMID",	"Number of macro identifiers");
	v[em_nid]		= MetricDetails(1, 1, 1, "NID",		"Total number of object and object-like identifiers");
	v[em_nupid]		= MetricDetails(1, 1, 1, "NUPID",	"Number of unique project-scope identifiers");
	v[em_nufid]		= MetricDetails(1, 1, 1, "NUFID",	"Number of unique file-scope (static) identifiers");
	v[em_numid]		= MetricDetails(1, 0, 1, "NUMID",	"Number of unique macro identifiers");
	v[em_nuid]		= MetricDetails(1, 1, 1, "NUID",	"Number of unique object and object-like identifiers");
	v[em_nlabid]		= MetricDetails(0, 0, 1, "INTERNAL",	"Number of label identifiers");
	v[em_nlabel]		= MetricDetails(1, 1, 1, "NLABEL",	"Number of goto labels");
	return v;
};

// Global metrics
IdMetricsSummary id_msum;

/*
 * Called when analyzing the preprocessed file for all file characters
 * appart from identifiers.
 */
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
IdMetricsSummary::add_pre_cpp_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].all[IdMetricsSet::pp_pre].add(ec, add_one());
}

void
IdMetricsSummary::add_post_cpp_id(Eclass *ec)
{
	rw[ec->get_attribute(is_readonly)].all[IdMetricsSet::pp_post].add(ec, add_one());
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
		"<th>" "Pre-cpp<br>total ids" "</th>"
		"<th>" "Post-cpp<br>total ids" "</th>"
		"<th>" "Distinct ids" "</th>"
		"<th>" "Avg len" "</th>"
		"<th>" "Min len" "</th>"
		"<th>" "Max len" "</th></tr>\n";

	const string tdnum("<td style='text-align: right;'>");
	o << "<tr><td>" "All identifiers" "</td>"
	    << tdnum << m.all[IdMetricsSet::pp_pre].total << "</td>"
	    << tdnum << m.all[IdMetricsSet::pp_post].total << "</td>"
	    << tdnum << m.once.total << "</td>"
	    << tdnum << avg(m.len.total, m.once.total) << "</td>"
	    << tdnum << m.minlen.total << "</td>"
	    << tdnum << m.maxlen.total << "</td>"
	    << "</tr>\n";
	for (int i = is_readonly + 1; i < attr_end; i++) {
		o << "<tr><td>" << Attributes::name(i) << "</td>"
		    << tdnum << m.all[IdMetricsSet::pp_pre].get_count(i) << "</td>"
		    << tdnum << m.all[IdMetricsSet::pp_post].get_count(i) << "</td>"
		    << tdnum << m.once.get_count(i) << "</td>"
		    << tdnum << avg(m.len.get_count(i), m.once.get_count(i)) << "</td>"
		    << tdnum << m.minlen.get_count(i) << "</td>"
		    << tdnum << m.maxlen.get_count(i) << "</td>"
		    << "</tr>\n";
	}
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

void
Metrics::process_queued_identifiers()
{
	for (auto i = queued_identifiers.begin(); i != queued_identifiers.end();
	    ++i) {
		const auto& tparts(i->constituents());
		for (auto tpart_it = tparts.begin(); tpart_it != tparts.end();
		    ++tpart_it) {
			process_identifier(
			    tpart_it->get_len(),
			    tpart_it->get_tokid().get_ec()
			);

			id_msum.add_post_cpp_id(tpart_it->get_tokid().get_ec());
		}
	}
}

double
Metrics::get_metric(int n) const
{
	switch (n) {
	// Most metrics are simply looked up
	default:
		return count[n];
	// A few are calculated dynamically
	case em_nlabel:	// Number of goto labels
		return (get_metric(em_nlabid) - get_metric(em_ngoto));
	}
}

// Summarize the identifiers collected by process_identifier
void
Metrics::summarize_identifiers()
{
	process_queued_identifiers();

	count[em_nupid] = pids.size();
	pids.clear();
	count[em_nufid] = fids.size();
	fids.clear();
	count[em_numid] = mids.size();
	mids.clear();
	count[em_nuid] = ids.size();
	ids.clear();
}

// Called for every identifier
void
Metrics::process_identifier(int len, Eclass *ec)
{
	count[em_nchar] += len;
	currlinelen += len;

	if (!ec)
		return;
	if (ec->get_attribute(is_lscope)) {
		count[em_npid]++;
		pids.insert(ec);
	}
	if (ec->get_attribute(is_cscope)) {
		count[em_nfid]++;
		fids.insert(ec);
	}
	if (ec->get_attribute(is_macro)) {
		count[em_nmid]++;
		mids.insert(ec);
	}
	count[em_nlabid] += ec->get_attribute(is_label);
	if (ec->get_attribute(is_ordinary) || ec->get_attribute(is_macro)) {
		count[em_nid]++;
		ids.insert(ec);
	}
}
