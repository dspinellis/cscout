/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: funmetrics.cpp,v 1.6 2007/08/14 13:58:58 dds Exp $
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

vector<bool> &FunctionMetrics::is_operator_map = make_is_operator();

MetricDetails FunctionMetrics::metric_details[] = {
// BEGIN AUTOSCHEMA FunctionMetrics
	// Elements counted before the preprocessor token tap
	{ em_nsemi,		"NSEMI",		"Number of statements or declarations"},
	{ em_nop,		"NOP",			"Number of operators"},
	{ em_nuop,		"NUOP",			"Number of unique operators"},
	{ em_nnconst,		"NNCONST",		"Number of numeric constants"},
	{ em_nclit,		"NCLIT",		"Number of character literals"},
	{ em_ncc2op,		"INTERNAL",		"Number of operators contributing to cc2: &&, ||, ?:"},
	// Keywords counted during identifier processing
	{ em_nif,		"NIF",			"Number of if statements", 	"if"},
	{ em_nelse,		"NELSE",		"Number of else clauses",	"else"},
	{ em_nswitch,		"NSWITCH",		"Number of switch statements",	"switch"},
	{ em_ncase,		"NCASE",		"Number of case labels",	"case"},
	{ em_ndefault,		"NDEFAULT",		"Number of default labels",	"default"},
	{ em_nbreak,		"NBREAK",		"Number of break statements",	"break"},
	{ em_nfor,		"NFOR",			"Number of for statements",	"for"},
	{ em_nwhile,		"NWHILE",		"Number of while statements",	"while"},
	{ em_ndo,		"NDO",			"Number of do statements",	"do"},
	{ em_ncontinue,		"NCONTINUE",		"Number of continue statements","continue"},
	{ em_ngoto,		"NGOTO",		"Number of goto statements",	"goto"},
	{ em_nreturn,		"NRETURN",		"Number of return statements",	"break"},
	// Identifiers categorized during identifier processing
	{ em_npid,		"NPID",			"Number of project-scope identifiers"},
	{ em_nfid,		"NFID",			"Number of file-scope (static) identifiers"},
	{ em_nmid,		"NMID",			"Number of macro identifiers"},
	{ em_nid,		"NID",			"Total number of object and object-like identifiers"},
	{ em_nuid,		"NUID",			"Number of unique object and object-like identifiers"},
	{ em_nlabid,		"INTERNAL",		"Number of label identifiers"},
	// During processing (once based on processed)
	{ em_nparam,		"NPARAM",		"Number of parameters"},
	// Metrics dynamically derived
	{ em_nlabel,		"NLABEL",		"Number of goto labels"},
	{ em_fanin,		"FANIN",		"Fan-in (number of calling functions)"},
	{ em_fanout,		"FANOUT",		"Fan-out (number of called functions)"},
	{ em_ccycl1,		"CCYCL1",		"Cyclomatic complexity (control statements)"},
	{ em_ccycl2,		"CCYCL2",		"Extended cyclomatic complexity (includes branching operators)"},
	{ em_ccycl3,		"CCYCL3",		"Maximum cyclomatic complexity (includes branching operators and all switch branches)"},
	// Metrics after this point are hardcoded to be stored in the database as REAL
	{ em_cstruc,		"CSCRUC",		"Structure complexity (Henry and Kafura)"},	// REAL
	{ em_chal,		"CHAL",			"Halstead complexity"},				// REAL
	{ em_iflow,		"IFLOW",		"Information flow metric (Henry and Selig)"},	// REAL
// END AUTOSCHEMA FunctionMetrics
};

double
FunctionMetrics::get_metric(int n) const
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
		return (get_metric(em_nop) +
			get_metric(em_nid) +
			get_metric(em_nnconst) +
			get_metric(em_nclit)) *
		    log2(get_metric(em_nuop) +
		        get_metric(em_nuid) +
			get_metric(em_nnconst) +
			get_metric(em_nclit));
	}
}

// Add operator op to int-indexed map v
inline void
FunctionMetrics::add_operator(vector<bool> &v, unsigned op)
{
	if (op >= v.size())
		v.resize(op + 1, false);
	v[op] = true;
}

// Create an integer-indexed map indicating which tokens represent operators
vector<bool> &
FunctionMetrics::make_is_operator()
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
FunctionMetrics::summarize_operators()
{
	if (processed)
		return;
	count[em_nuop] = operators.size();
	operators.clear();
}
