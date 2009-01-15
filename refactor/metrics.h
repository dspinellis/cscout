/*
 * (C) Copyright 2002-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
 * $Id: metrics.h,v 1.33 2009/01/15 14:32:57 dds Exp $
 */

#ifndef METRICS_
#define METRICS_

#include <string>
#include <vector>
#include <limits>
#include <ostream>

using namespace std;

#include "attr.h"
#include "error.h"

class Eclass;

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
	int id;			// Metric identifier
	string dbfield;		// Database field name
	string name;		// User-visible name
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

	// Metrics we collect
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
		em_npptoken,		// Number of preprocessed tokens
		em_nctoken,		// Number of compiled tokens
		metric_max
	};

	// Called for all file characters appart from identifiers
	void process_char(char c);
	// Called for every identifier
	void process_id(const string &s, Eclass *ec);
	// Called when encountering unprocessed lines
	void add_unprocessed() { count[em_nuline]++; }

	// Manipulate the processing-based metrics
	void add_ppdirective() { if (!processed) count[em_nppdirective]++; }
	void add_ppcond() { if (!processed) count[em_nppcond]++; }
	void add_ppfmacro() { if (!processed) count[em_nppfmacro]++; }
	void add_ppomacro() { if (!processed) count[em_nppomacro]++; }
	void add_pptoken() { if (!processed) count[em_npptoken]++; }
	void add_ctoken() { if (!processed) count[em_nctoken]++; }

	void done_processing() { processed = true; }
	bool is_processed() const { return processed; }

	// Get methods
	enum e_cfile_state get_state() { return cstate; }
	// Generic
	double get_metric(int n) const { return count[n]; }
	void set_metric(int n, int val) { count[n] = val; }

	// Call the specified metrics function for the current file and function
	static void call_metrics(void (Metrics::*fun)());

	// Return true if the specified metric shall not appear in the UI/RDBMS
	template <class M>
	static bool is_internal(int n) { return get_dbfield<M>(n) == "INTERNAL"; }

	// Return a reference to the details of the specified metric
	template <class M> static const MetricDetails & get_detail(int n);

	// Return the database field name of the specified metric
	template <class M> static const string & get_dbfield(int n);

	// Return the metric name of the specified metric
	template <class M> static const string & get_name(int n);
};

class Eclass;
class FileMetricsSet;
class IdMetricsSet;

// A class for keeping taly of various identifier type counts
class IdCount {
private:
	double total;
	vector <double> count;		// Counts per identifier attribute
public:
	IdCount() :
		total(0),
		count(attr_end, 0)
	{}
	double get_count(int i) { return count[i]; }
	// Adjust class members according to the attributes of EC
	// using function object f
	template <class UnaryFunction>
	void add(Eclass *ec, UnaryFunction f);
	friend ostream& operator<<(ostream& o, const IdMetricsSet &m);
};

class Fileid;
class IdMetricsSet;


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

// Global metrics
extern IdMetricsSummary id_msum;

struct add_one : public unary_function<double, double>
{
      double operator()(double x) { return x + 1; }
};

struct add_n : public unary_function<double, double>
{
      double n;
      add_n(double add) { n = add; }
      double operator()(double x) { return x + n; }
};

struct set_max : public unary_function<double, double>
{
      double n;
      set_max(double newval) { n = newval; }
      double operator()(double x) { return x > n ? x : n; }
};

struct set_min : public unary_function<double, double>
{
      double n;
      set_min(double newval) { n = newval; }
      double operator()(double x) { return (x < n && x > 0) ? x : n; }
};

struct get_max : public binary_function<double, double, double>
{
      double operator()(double x, double y) { return (x < y) ? y : x; }
};

struct get_min : public binary_function<double, double, double>
{
      double operator()(double x, double y) { return (x > y) ? y : x; }
};

// Return the average of a sum v over n values as a string
string avg(double v, double n);

// Return a reference to the details of the specified metric
template <class M>
const MetricDetails &
Metrics::get_detail(int n)
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
Metrics::get_dbfield(int n)
{
	if (n < Metrics::metric_max)
		return get_detail<Metrics>(n).dbfield;
	else
		return get_detail<M>(n).dbfield;
}

// Return the metric name of the specified metric
template <class M>
const string &
Metrics::get_name(int n)
{
	if (n < Metrics::metric_max)
		return get_detail<Metrics>(n).name;
	else
		return get_detail<M>(n).name;
}

template <class M, class E> class MetricsRange;

// Counting metric details
template <class M, class E>
class MetricsCount {
private:
	// Totals for all files
	int nelement;		// Number of unique files
	vector <double> count;	// File metric counts
public:
	MetricsCount(double v = 0) :
		nelement(0),
		count(M::metric_max, v)
	{}
	double get_metric(int i)  const { return count[i]; }
	// Add the details of file fi
	template <class BinaryFunction>
	// Update metrics summary
	void add(E &fi, BinaryFunction f) {
		nelement++;
		for (int i = 0; i < M::metric_max; i++)
			count[i] = f(fi.metrics().get_metric(i), count[i]);
	}
	int get_nelement() const { return nelement; }
	template <class MM, class EE>
	friend ostream& operator<<(ostream& o, const MetricsRange<M, E> &m);
};


// Tally the range and sum of the metrics
template <class M, class E>
class MetricsRange {
public:
	MetricsCount<M, E> total;	// Metric details, total
	MetricsCount<M, E> min;	// Metric details, min across files
	MetricsCount<M, E> max;	// Metric details, max across files
	MetricsRange() :
		//min(INT_MAX)
		// When my Linux upgrades from gcc 2.96
		min(numeric_limits<double>::max())
	{}
	template <class MM, class EE>
	friend ostream& operator<<(ostream& o, const MetricsRange &m);
	double get_total(int i) { return total.get_metric(i); }
};

template <class M, class E>
ostream&
operator<<(ostream& o, const MetricsRange<M, E> &m)
{
	o << "Number of elements: " << m.total.get_nelement() << "<p>\n";
	if (m.total.get_nelement() == 0)
		return o;
	o << "<table class='metrics'>"
		"<tr><th>" "Metric" "</th>"
		"<th>" "Total" "</th>"
		"<th>" "Min" "</th>"
		"<th>" "Max" "</th>"
		"<th>" "Avg" "</th></tr>\n";
	for (int i = 0; i < M::metric_max; i++)
		if (!Metrics::is_internal<M>(i))
			o << "<tr><td>" << Metrics::get_name<M>(i) << "</td>"
			    "<td>" << m.total.get_metric(i) << "</td>"
			    "<td>" << m.min.get_metric(i) << "</td>"
			    "<td>" << m.max.get_metric(i) << "</td>"
			    "<td>" << avg(m.total.get_metric(i), m.total.get_nelement()) << "</td></tr>\n";
	o << "</table>\n";
	return o;
}

#endif /* METRICS_ */
