/*
 * (C) Copyright 2002 Diomidis Spinellis.
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
 * process_char() or process_id() while going through each file
 * msum.add_unique_id once() for every EC
 * msum.add_id() for each identifier having an EC
 * summarize_files() at the end of processing
 *
 * $Id: filemetrics.h,v 1.23 2007/08/13 15:56:44 dds Exp $
 */

#ifndef FILEMETRICS_
#define FILEMETRICS_

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

	template <class M> friend const struct MetricDetails &Metrics::get_detail(int n);
};

// Counting file details
class FileCount {
private:
	// Totals for all files
	int nfile;		// Number of unique files
	vector <int> count;	// File metric counts
public:
	FileCount(int v = 0) :
		nfile(0),
		count(FileMetrics::metric_max, v)
	{}
	int get_metric(int i)  const { return count[i]; }
	// Add the details of file fi
	template <class BinaryFunction>
	void add(Fileid &fi, BinaryFunction f);
	friend ostream& operator<<(ostream& o, const FileMetricsSet &m);
};
// One such set is kept for readable and writable files
class FileMetricsSet {
	friend class FileMetricsSummary;
	FileCount total;// File details, total
	FileCount min;	// File details, min across files
	FileCount max;	// File details, max across files
public:
	FileMetricsSet();
	friend ostream& operator<<(ostream& o, const FileMetricsSet &m);
	int get_total(int i) { return total.get_metric(i); }
};

// This can be kept per project and globally
class FileMetricsSummary {
	FileMetricsSet rw[2];			// For read-only and writable cases
public:
	// Create file-based summary
	void summarize_files();
	friend ostream& operator<<(ostream& o,const FileMetricsSummary &ms);
	int get_total(int i) { return rw[0].get_total(i) + rw[1].get_total(i); }
	int get_readonly(int i) { return rw[1].get_total(i); }
	int get_writable(int i) { return rw[0].get_total(i); }
};

extern FileMetricsSummary file_msum;
#endif /* FILEMETRICS_ */
