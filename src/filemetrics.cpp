/*
 * (C) Copyright 2002-2024 Diomidis Spinellis
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
#include "pltoken.h"
#include "eclass.h"
#include "fchar.h"
#include "filedetails.h"

MetricDetails FileMetrics::metric_details[] = {
// BEGIN AUTOSCHEMA FileMetrics
	{ em_ncopies,		"NCOPIES",		"Number of copies of the file"},
	{ em_npfunction,	"NPFUNCTION",		"Number of defined project-scope functions"},
	{ em_nffunction,	"NFFUNCTION",		"Number of defined file-scope (static) functions"},
	{ em_npvar,		"NPVAR",		"Number of defined project-scope variables"},
	{ em_nfvar,		"NFVAR",		"Number of defined file-scope (static) variables"},
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
		rw[Filedetails::get_attribute(*i, is_readonly)].total.add((*i), plus<double>());
		rw[Filedetails::get_attribute(*i, is_readonly)].min.add((*i), get_min());
		rw[Filedetails::get_attribute(*i, is_readonly)].max.add((*i), get_max());
	}
}

ostream&
operator<<(ostream& o, const FileMetricsSummary &ms)
{
	o << "<h2>Writable Files</h2>\n" << ms.rw[false];
	o << "<h2>Read-only Files</h2>\n" << ms.rw[true];
	return o;
}
