/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: filemetrics.cpp,v 1.27 2007/08/19 12:36:20 dds Exp $
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
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "filemetrics.h"
#include "fileid.h"
#include "tokid.h"
#include "tokmap.h"
#include "eclass.h"
#include "fchar.h"

MetricDetails FileMetrics::metric_details[] = {
// BEGIN AUTOSCHEMA FileMetrics
	{ em_nstatement,	"NSTATEMENT",		"Number of statements"},
	{ em_ncopies,		"NCOPIES",		"Number of copies of the file"},
	{ em_npfunction,	"NPFUNCTION",		"Defined project-scope functions"},
	{ em_nffunction,	"NFFUNCTION",		"Defined file-scope (static) functions"},
	{ em_npvar,		"NPVAR",		"Defined project-scope variables"},
	{ em_nfvar,		"NFVAR",		"Defined file-scope (static) variables"},
	{ em_naggregate,	"NAGGREGATE",		"Number of complete aggregate (struct/union) declarations"},
	{ em_namember,		"NAMEMBER",		"Number of declared aggregate (struct/union) members"},
	{ em_nenum,		"NENUM",		"Number of complete enumeration declarations"},
	{ em_nemember,		"NEMEMBER",		"Number of declared enumeration elements"},
	{ em_nincfile,		"NINCFILE",		"Number of directly included files"},
// END AUTOSCHEMA FileMetrics
};

// Global metrics
FileMetricsSummary file_msum;

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

ostream&
operator<<(ostream& o, const FileMetricsSummary &ms)
{
	o << "<h2>Writable Files</h2>\n" << ms.rw[false];
	o << "<h2>Read-only Files</h2>\n" << ms.rw[true];
	return o;
}
