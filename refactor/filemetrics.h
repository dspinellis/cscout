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
 * $Id: filemetrics.h,v 1.21 2007/08/11 12:47:24 dds Exp $
 */

#ifndef METRICS_
#define METRICS_


// States while processing characters
enum e_cfile_state {
	s_normal,
	s_saw_slash,		// After a / character
	s_saw_str_backslash,	// After a \ character in a string
	s_saw_chr_backslash,	// After a \ character in a character
	s_cpp_comment,		// Inside C++ comment
	s_block_comment,	// Inside C block comment
	s_block_star,		// Found a * in a block comment
	s_string,		// Inside a string
	s_char,			// Inside a character
};

// Details for each metric
struct MetricDetails {
	int id;		// Metric identifier
	string dbfield;	// Database field name
	string name;	// User-visible name
};

// Metrics for regions of code (files and functions)
class Metrics {
private:
	int currlinelen;
	enum e_cfile_state cstate;
	static MetricDetails metric_details[];

protected:
	vector <int> count;	// Metric counts

	// Helper variables
	bool processed;		// True after an element has been processed
public:
	Metrics() :
		currlinelen(0),
		cstate(s_normal),
		processed(false)
	{}

	// Matrics we collect
	enum e_metric {
	// During post-processing
		em_nchar,		// Number of characters
		em_nccomment,		// Comment characters
		em_nspace,		// Space characters
		em_nlcomment,		// Number of line comments
		em_nbcomment,		// Number of block comments
		em_nline,		// Number of lines
		em_maxlinelen,		// Maximum number of characters in a line
		em_nstring,		// Number of character strings
		em_nuline,		// Number of unprocessed lines

	// During processing (once based on processed)
		em_nppdirective,	// Number of cpp directives
		em_nppcond,		// Number of processed cpp conditionals (ifdef, if, elif)
		em_nppfmacro,		// Number of defined cpp function-like macros
		em_nppomacro,		// Number of defined cpp object-like macros
		metric_max
	};

	// Called for all file characters appart from identifiers
	void process_char(char c);
	// Called for every identifier
	void process_id(const string &s);
	// Called when encountering unprocessed lines
	void add_unprocessed() { count[em_nuline]++; }

	// Manipulate the processing-based metrics
	void add_ppdirective() { if (!processed) count[em_nppdirective]++; }
	void add_ppcond() { if (!processed) count[em_nppcond]++; }
	void add_ppfmacro() { if (!processed) count[em_nppfmacro]++; }
	void add_ppomacro() { if (!processed) count[em_nppomacro]++; }

	void done_processing() { processed = true; }
	bool is_processed() const { return processed; }

	// Get methods
	enum e_cfile_state get_state() { return cstate; }
	// Generic
	int get_metric(int n) const { return count[n]; }
	void set_metric(int n, int val) { count[n] = val; }

	template <class M> friend const struct MetricDetails &get_detail(int n);
};

class FileMetrics : public Metrics {
private:
	static MetricDetails metric_details[];

public:
	FileMetrics() { count.resize(metric_max, 0); }

	// Matrics we collect
	enum e_metric {
	// During post-processing
		em_ncopies =		// Number of copies of the file
			Metrics::metric_max,
	// During processing (once based on processed)
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

	template <class M> friend const struct MetricDetails &get_detail(int n);
};

class FunctionMetrics : public Metrics {
private:
	static MetricDetails metric_details[];

public:
	FunctionMetrics() { count.resize(metric_max, 0); }

	// Matrics we collect
	enum e_metric {
#ifdef ndef
	// During post-processing
		em_nstatement =		// Number of statements
			Metrics::metric_max,
		em_nif,			// Number of if keywords
		em_nelse,		// Number of else keywords
		em_nwhile,		// Number of while keywords
		em_ndo,			// Number of do keywords
		em_nswitch,		// Number of switch keywords
		em_ncase,		// Number of switch keywords
	// During processing (once based on processed)
		em_param,		// Number of parameters
#endif
		metric_max = Metrics::metric_max // XXX
	};

	template <class M> friend const struct MetricDetails &get_detail(int n);
};

// Return a reference to the details of the specified metric
template <class M>
static const struct MetricDetails &
get_detail(int n)
{
	static const MetricDetails unknown = {0, "UNKNOWN", "UNKNOWN"};

	csassert(n < M::metric_max);
	for (int i = 0 ; i < M::metric_max; i++)
		if (M::metric_details[i].id == n)
			return (M::metric_details[i]);
	csassert(0);
	return (unknown);
}

// Return the database field name of the specified metric
template <class M>
const string &
get_dbfield(int n)
{
	if (n < Metrics::metric_max)
		return get_detail<Metrics>(n).dbfield;
	else
		return get_detail<M>(n).dbfield;
}

// Return the metric name of the specified metric
template <class M>
const string &
get_name(int n)
{
	if (n < Metrics::metric_max)
		return get_detail<Metrics>(n).name;
	else
		return get_detail<M>(n).name;
}

class Eclass;
class FileMetricsSet;
class IdMetricsSet;

// A class for keeping taly of various identifier type counts
class IdCount {
private:
	int total;
	vector <int> count;		// Counts per identifier attribute
public:
	IdCount() :
		total(0),
		count(attr_end, 0)
	{}
	int get_count(int i) { return count[i]; }
	// Adjust class members according to the attributes of EC
	// using function object f
	template <class UnaryFunction>
	void add(Eclass *ec, UnaryFunction f);
	friend ostream& operator<<(ostream& o, const IdMetricsSet &m);
};

class Fileid;
class IdMetricsSet;

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
	int get_metric(int i) { return count[i]; }
	// Add the details of file fi
	template <class BinaryFunction>
	void add(Fileid &fi, BinaryFunction f);
	friend ostream& operator<<(ostream& o, const FileMetricsSet &m);
};

// One such set is kept for readable and writable identifiers
class IdMetricsSet {
	friend class IdMetricsSummary;
	IdCount once;	// Each identifier EC is counted once
	IdCount len;	// Use the len of each EC
	IdCount maxlen;	// Maximum length for each type
	IdCount minlen;	// Minimum length for each type
	IdCount all;	// Each identifier counted for every occurance in a file
public:
	friend ostream& operator<<(ostream& o, const IdMetricsSet &m);
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
class IdMetricsSummary {
	IdMetricsSet rw[2];			// For writable (0) and read-only (1) cases
public:
	// Called for every identifier occurence
	void add_id(Eclass *ec);
	// Called for every unique identifier occurence (EC)
	void add_unique_id(Eclass *ec);
	friend ostream& operator<<(ostream& o,const IdMetricsSummary &ms);
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

// Global metrics
extern IdMetricsSummary id_msum;
extern FileMetricsSummary file_msum;

#endif /* METRICS_ */
