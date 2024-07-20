/*
 * (C) Copyright 2002-2015 Diomidis Spinellis
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
 * A metrics and a metrics summary container.
 *
 * One metrics object is created and updated for every file.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing
 * overhead will be incured exactly once for each file.
 *
 * During postprocessing call:
 * process_char() or process_identifier() while going through each file
 * msum.add_unique_id once() for every EC
 * msum.add_id() for each identifier having an EC
 * summarize_files() at the end of processing
 *
 */

#ifndef FILEMETRICS_
#define FILEMETRICS_

#include "metrics.h"

class FileMetrics : public Metrics {
private:
	static MetricDetails metric_details[];

public:
	FileMetrics() { count.resize(metric_max, 0); }

	// Metrics we collect
	enum e_metric {
	// During post-processing
		em_ncopies =		// Number of copies of the file
			Metrics::metric_max,
	// During pre-processing or parsing (once based on processed)
		em_nstatement,		// Number of statements
		em_npfunction,		// Defined project-scope functions
		em_nffunction,		// Defined file-scope (static) functions
		em_npvar,		// Defined project-scope variables
		em_nfvar,		// Defined file-scope (static) variables
		em_naggregate,		// Number of complete structure / union declarations
		em_namember,		// Number of declared aggregate members
		em_nenum,		// Number of complete enumeration declarations
		em_nemember,		// Number of declared enumeration elements
		em_nincfile,		// Number of directly included files
		metric_max
	};

	// Called to set the number of other identical files
	void set_ncopies(int n) { count[em_ncopies] = n; }

	// Manipulate the processing-based metrics
	void add_statement() { if (!processed) count[em_nstatement]++; }
	void add_incfile() { if (!processed) count[em_nincfile]++; }

	// Increment the number of functions for the file being processed
	void add_function(bool is_file_scoped) {
		if (processed)
			return;
		if (is_file_scoped)
			count[em_nffunction]++;
		else
			count[em_npfunction]++;
	}
	void add_aggregate() { if (!processed) count[em_naggregate]++; }
	void add_amember() { if (!processed) count[em_namember]++; }
	void add_enum() { if (!processed) count[em_nenum]++; }
	void add_emember() { if (!processed) count[em_nemember]++; }
	void add_pvar() { if (!processed) count[em_npvar]++; }
	void add_fvar() { if (!processed) count[em_nfvar]++; }

	int get_int_metric(int n) const { return count[n]; }
	virtual ~FileMetrics() {}

	template <class M> friend const MetricDetails &Metrics::get_detail(int n);
};

// This can be kept per project and globally
class FileMetricsSummary {
	MetricsRange<FileMetrics, Fileid> rw[2];			// For read-only and writable cases
public:
	// Create file-based summary
	void summarize_files();
	friend ostream& operator<<(ostream& o,const FileMetricsSummary &ms);
	double get_total(int i) { return rw[0].get_total(i) + rw[1].get_total(i); }
	double get_readonly(int i) { return rw[1].get_total(i); }
	double get_writable(int i) { return rw[0].get_total(i); }
};

extern FileMetricsSummary file_msum;

#endif /* FILEMETRICS_ */
