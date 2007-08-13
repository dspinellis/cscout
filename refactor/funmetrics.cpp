/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: funmetrics.cpp,v 1.2 2007/08/13 15:56:44 dds Exp $
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


MetricDetails FunctionMetrics::metric_details[] = {
// BEGIN AUTOSCHEMA FunctionMetrics
	// Elements counted before the preprocessor token tap
	{ em_nsemi,		"NSEMI",		"Number of statements or declarations"},
	{ em_nop,		"NOP",			"Number of operators"},
	{ em_nuop,		"NUOP",			"Number of unique operators"},
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
		return (em_nlabid - em_ngoto);
	case em_fanin:	// Fan-in (number of calling functions)
		return call->get_num_caller();
	case em_fanout:	// Fan-out (number of called functions)
		return call->get_num_call();
	case em_ccycl1:	// Cyclomatic complexity (control statements)
		return (em_nif + em_nswitch + em_nfor + em_nwhile + em_ndo + 1);
	case em_ccycl2:	// Extended cyclomatic complexity (including branching operators)
		return get_metric(em_ccycl1) + em_ncc2op;
	case em_ccycl3:	// Cyclomatic complexity (including switch branches)
		return get_metric(em_ccycl2) - em_nswitch + em_ncase;
	case em_cstruc:	// Structure complexity (Henry and Kafura)
#define sqr(x) ((x) * (x))
		return sqr((double)get_metric(em_fanin) * (double)get_metric(em_fanout));
	case em_iflow:	// Information flow metric
		return get_metric(em_cstruc) * get_metric(em_ccycl1);
	case em_chal:	// Halstead complexity
#define log2(x) (log(x) / log(2.))
		return (get_metric(em_nop) + get_metric(em_nid)) *
			log2(get_metric(em_nuop) + get_metric(em_nuid));
	}
}
