/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: funmetrics.cpp,v 1.15 2007/08/19 09:07:54 dds Exp $
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
#include <climits>		// INT_MAX
//#include <limits>		// numeric_limits
#include <errno.h>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "funmetrics.h"
#include "filemetrics.h"
#include "fileid.h"
#include "tokid.h"
#include "tokmap.h"
#include "eclass.h"
#include "fchar.h"
#include "token.h"
#include "call.h"
#include "ytab.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "pltoken.h"

vector<bool> &FunMetrics::is_operator_map = make_is_operator();
FunMetrics::KeywordMap &FunMetrics::keyword_map = make_keyword_map();

MetricDetails FunMetrics::metric_details[] = {
// BEGIN AUTOSCHEMA FunMetrics
	// Elements counted at the token tap before the preprocessor
	{ em_nstmt,		"NSTMT",		"Number of statements or declarations"},
	{ em_nop,		"NOP",			"Number of operators"},
	{ em_nuop,		"NUOP",			"Number of unique operators"},
	{ em_nnconst,		"NNCONST",		"Number of numeric constants"},
	{ em_nclit,		"NCLIT",		"Number of character literals"},
	{ em_ncc2op,		"INTERNAL",		"Number of operators contributing to cc2: &&, ||, ?:"},
	// Keywords counted at the token tap before the preprocessor
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
	// During processing (once based on processed)
	{ em_ngnsoc,		"NGNSOC",		"Number of global namespace occupants at function's top"},
	{ em_nparam,		"NPARAM",		"Number of parameters"},
	{ em_maxnest,		"MAXNEST",		"Maximum level of statement nesting"},
	// Metrics dynamically derived
	{ em_nlabel,		"NLABEL",		"Number of goto labels"},
	{ em_fanin,		"FANIN",		"Fan-in (number of calling functions)"},
	{ em_fanout,		"FANOUT",		"Fan-out (number of called functions)"},
	{ em_ccycl1,		"CCYCL1",		"Cyclomatic complexity (control statements)"},
	{ em_ccycl2,		"CCYCL2",		"Extended cyclomatic complexity (includes branching operators)"},
	{ em_ccycl3,		"CCYCL3",		"Maximum cyclomatic complexity (includes branching operators and all switch branches)"},
	// Dynamically derived metrics after this point are hardcoded to be stored in the database as REAL
	{ em_cstruc,		"CSCRUC",		"Structure complexity (Henry and Kafura)"},	// REAL
	{ em_chal,		"CHAL",			"Halstead complexity"},				// REAL
	{ em_iflow,		"IFLOW",		"Information flow metric (Henry and Selig)"},	// REAL
// END AUTOSCHEMA FunMetrics
};

double
FunMetrics::get_metric(int n) const
{
	switch (n) {
	// Most metrics are simply looked up
	default:
		return count[n];
	// A few are calculated dynamically
	case em_nlabel:	// Number of goto labels
		return (get_metric(em_nlabid) - get_metric(em_ngoto));
	case em_fanin:	// Fan-in (number of calling functions)
		return call->get_num_caller();
	case em_fanout:	// Fan-out (number of called functions)
		return call->get_num_call();
	case em_ccycl1:	// Cyclomatic complexity (control statements)
		return (get_metric(em_nif) + get_metric(em_nswitch) +
		    get_metric(em_nfor) + get_metric(em_nwhile) + get_metric(em_ndo) + 1);
	case em_ccycl2:	// Extended cyclomatic complexity (including branching operators)
		return get_metric(em_ccycl1) + get_metric(em_ncc2op);
	case em_ccycl3:	// Cyclomatic complexity (including switch branches)
		return get_metric(em_ccycl2) - get_metric(em_nswitch) + get_metric(em_ncase);
	case em_cstruc:	// Structure complexity (Henry and Kafura)
#define sqr(x) ((x) * (x))
		return sqr((double)get_metric(em_fanin) * (double)get_metric(em_fanout));
	case em_iflow:	// Information flow metric
		return get_metric(em_cstruc) * get_metric(em_ccycl1);
	case em_chal:	// Halstead complexity
#define log2(x) (log(x) / log(2.))
		// We consider numeric constants and character literals to be unique operands
		double logarg = get_metric(em_nuop) +
		        get_metric(em_nuid) +
			get_metric(em_nnconst) +
			get_metric(em_nclit);
		if (logarg == 0)
			return 0;
		else
			return (get_metric(em_nop) +
				get_metric(em_nid) +
				get_metric(em_nnconst) +
				get_metric(em_nclit)) *
			    log2(logarg);
	}
}

// Add operator op to int-indexed map v
inline void
FunMetrics::add_operator(vector<bool> &v, unsigned op)
{
	if (op >= v.size())
		v.resize(op + 1, false);
	v[op] = true;
}

// Create an integer-indexed map indicating which tokens represent operators
vector<bool> &
FunMetrics::make_is_operator()
{
	static vector<bool> isop;	// Tokens that are operators

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

// Summarize the operators collected by process_token
void
FunMetrics::summarize_operators()
{
	if (processed)
		return;
	count[em_nuop] = operators.size();
	operators.clear();
}

// Summarize the identifiers collected by process_id
void
FunMetrics::summarize_identifiers()
{
	count[em_nupid] = pids.size();
	pids.clear();
	count[em_nufid] = fids.size();
	fids.clear();
	count[em_numid] = mids.size();
	mids.clear();
	count[em_nuid] = ids.size();
	ids.clear();
}

// Initialize map
FunMetrics::KeywordMap &
FunMetrics::make_keyword_map()
{
	static KeywordMap km;

	km.insert(KeywordMap::value_type("if", em_nif));
	km.insert(KeywordMap::value_type("else", em_nelse));
	km.insert(KeywordMap::value_type("switch", em_nswitch));
	km.insert(KeywordMap::value_type("case", em_ncase));
	km.insert(KeywordMap::value_type("default", em_ndefault));
	km.insert(KeywordMap::value_type("break", em_nbreak));
	km.insert(KeywordMap::value_type("for", em_nfor));
	km.insert(KeywordMap::value_type("while", em_nwhile));
	km.insert(KeywordMap::value_type("do", em_ndo));
	km.insert(KeywordMap::value_type("continue", em_ncontinue));
	km.insert(KeywordMap::value_type("goto", em_ngoto));
	km.insert(KeywordMap::value_type("return", em_nreturn));

	return (km);
}

// Process a single token read from a file
void
FunMetrics::process_token(const Pltoken &t)
{
	csassert(!processed);
	int code = t.get_code();
	int em;
	switch (code) {
	case IDENTIFIER:
		em = keyword_metric(t.get_val());
		if (em != -1)
			count[em]++;
		switch (em) {
		case em_nwhile:
		case em_nswitch:
		case em_nif:
			/*
			 * while (x) y; and the rest are two statements
			 * We count one through the ";", we must count the
			 * other through the keyword.
			 */
			count[em_nstmt]++;
			break;
		case em_ndo:
			count[em_nstmt]++;
			// Don't count the "while" associated with a "do"
			count[em_nwhile]--;
			break;
		case em_nfor:
			count[em_nstmt]++;
			// Don't count the semicolons in for statements
			count[em_nstmt] -= 2;
			break;
		}
		break;
	case ';':
		count[em_nstmt]++;
		break;
	case PP_NUMBER:
		count[em_nnconst]++;
		break;
	case CHAR_LITERAL:
		count[em_nclit]++;
		break;
	case AND_OP:
	case OR_OP:
	case '?':
		count[em_ncc2op]++;
		break;
	}
	if (is_operator(code)) {
		count[em_nop]++;
		operators.insert(code);
	}
}

// Called for every identifier
void
FunMetrics::process_id(const string &s, Eclass *ec)
{
	Metrics::process_id(s, ec);
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
	count[em_nid] += ec->get_attribute(is_ordinary);
	count[em_nlabid] += ec->get_attribute(is_label);
	if (ec->get_attribute(is_ordinary) || ec->get_attribute(is_macro))
		ids.insert(ec);
}
