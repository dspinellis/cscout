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

vector <MetricDetails>
FileMetrics::metric_details_values()
{
	vector <MetricDetails> v(metric_max);

	// First elements come from Metrics
	copy(Metrics::get_metric_details_vector().begin(),
	   Metrics::get_metric_details_vector().end(), v.begin());
						// pre-cpp, post-cpp, file
	v[em_ncopies]		= MetricDetails(1, 0, 1, "NCOPIES",	"Number of copies of the file");
	v[em_nincfile]		= MetricDetails(1, 0, 1, "NINCFILE",	"Number of directly included files");
	v[em_npfunction]	= MetricDetails(0, 1, 1, "NPFUNCTION",	"Number of defined project-scope functions");
	v[em_nffunction]	= MetricDetails(0, 1, 1, "NFFUNCTION",	"Number of defined file-scope (static) functions");
	v[em_npvar]		= MetricDetails(0, 1, 1, "NPVAR",	"Number of defined project-scope variables");
	v[em_nfvar]		= MetricDetails(0, 1, 1, "NFVAR",	"Number of defined file-scope (static) variables");
	v[em_naggregate]	= MetricDetails(0, 1, 1, "NAGGREGATE",	"Number of complete aggregate (struct/union) declarations");
	v[em_namember]		= MetricDetails(0, 1, 1, "NAMEMBER",	"Number of declared aggregate (struct/union) members");
	v[em_nenum]		= MetricDetails(0, 1, 1, "NENUM",	"Number of complete enumeration declarations");
	v[em_nemember]		= MetricDetails(0, 1, 1, "NEMEMBER",	"Number of declared enumeration elements");
	return v;
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
