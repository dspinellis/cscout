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
 * A metrics and a metrics summary container.
 *
 * One metrics object is created and updated for every file.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing
 * overhead will be incured exactly once for each file.
 *
 * Before preprocessing call:
 * process_token(int code) for every token recognized
 *
 *
 * During postprocessing call:
 * process_char() or process_identifier() while going through each file
 * summarize_identifiers() at the end of each function/file
 * msum.add_unique_id once() for every EC
 * msum.add_id() for each identifier having an EC
 * summarize_files() at the end of processing
 *
 */

#ifndef METRICS_
#define METRICS_

#include <limits>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

#include "parse.tab.h"
#include "attr.h"
#include "error.h"
#include "ctoken.h"

class Eclass;
class Filedetails;
class Call;

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
class MetricDetails {
private:
	bool pre_cpp;	// True if the metric is applicable before the cpp
	bool post_cpp;	// True if the metric is applicable after the cpp
	bool file;	// True if the metric is applicable to files 
	string dbfield;	// Database field name
	string name;	// User-visible name

public:
	MetricDetails(bool pre_cpp, bool post_cpp, bool file,
	    const string& dbfield, const string& name) : pre_cpp(pre_cpp),
	    post_cpp(post_cpp), file(file), dbfield(dbfield),
	    name(name) {}

	// Default ctor for when we are setting the vector<MetricDetails> size
	MetricDetails() : pre_cpp(false), post_cpp(false), file(false),
	    dbfield(""), name("") {}

	// Getters
	bool is_pre_cpp() const { return pre_cpp; }
	bool is_post_cpp() const { return post_cpp; }
	bool is_file() const { return file; }
	string get_dbfield() const { return dbfield; }
	string get_name() const { return name; }
};

// Metrics for regions of code (files and functions)
class Metrics {
private:
	int currlinelen;
	int currstmtlen;
	int currbracenesting;
	int currbracknesting;
	enum e_cfile_state cstate;

	// Map from a metric to its details and its initializer
	static vector <MetricDetails> metric_details;
	static vector<MetricDetails> metric_details_values();

	vector <Ctoken> queued_identifiers;

	set <Eclass *> pids;			// Project-scope dentifiers used in the function/file
	set <Eclass *> fids;			// File-scope identifiers used in the function/file
	set <Eclass *> mids;			// Macro identifiers used in the function/file
	set <Eclass *> ids;			// Identifiers used in the function/file

	// Int-indexed map of tokens that are operators
	static vector<bool> is_operator_map;
	// Return true if token op is an operator
	static inline bool is_operator(unsigned op) { return op < is_operator_map.size() && is_operator_map[op]; }
	// Add an operator to the map
	static inline void add_operator(vector<bool> &v, unsigned op);
	// Initialize map
	static vector<bool> make_is_operator();

	// Process the queued identifiers
	void process_queued_identifiers();

protected:
	vector <int> count;	// Metric counts
	set <int> operators;	// Operators used in the function/file

	bool processed;		// True after an element has been processed

	static const vector <MetricDetails>& get_metric_details_vector() {
		return metric_details;
	}
public:
	Metrics() :
		currlinelen(0),
		currstmtlen(0),
		currbracenesting(0),
		currbracknesting(0),
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
		em_maxstmtlen,		// Maximum number of characters in a line
		em_maxstmtnest,		// Maximum level of statement nesting
		em_maxbracenest,	// Maximum level of brace nesting
		em_maxbracknest,	// Maximum level of bracket nesting
		em_nuline,		// Number of unprocessed lines

	// During processing (once based on processed)
		em_nppdirective,	// Number of cpp directives
		em_nppcond,		// Number of processed cpp conditionals (ifdef, if, elif)
		em_nppfmacro,		// Number of defined cpp function-like macros
		em_nppomacro,		// Number of defined cpp object-like macros
		em_ntoken,		// Number of tokens

	// Elements counted before the preprocessor token tap
		em_nstmt,	// Number of statements or declarations
		em_nop,		// Number of operators
		em_nuop,	// Number of unique operators
		em_nnconst,	// Number of numeric constants
		em_nclit,	// Number of character literals
		em_nstring,	// Number of character strings
		em_ncc2op,	// (INT) Number of operators contributing to cc2: &&, ||, ?:
		/*
		 * Keywords counted during identifier processing that takes
		 * place when each file is post-processed.
		 */
		em_nif,		// Number of if statements
		em_nelse,	// Number of else clauses
		em_nswitch,	// Number of switch statements
		em_ncase,	// Number of case labels
		em_ndefault,	// Number of default labels
		em_nbreak,	// Number of break statements
		em_nfor,	// Number of for statements
		em_nwhile,	// Number of while statements
		em_ndo,		// Number of do statements
		em_ncontinue,	// Number of continue statements
		em_ngoto,	// Number of goto statements
		em_nreturn,	// Number of return statements
		/*
		 * Identifiers (total and unique) categorized during
		 * identifier processing that takes place when each
		 * file is post-processed.
		 */
		// The following four lines must match the next four
		em_npid,	// Number of project-scope identifiers
		em_nfid,	// Number of file-scope (static) identifiers
		em_nmid,	// Number of macro identifiers
		em_nid,		// Total number of object and object-like identifiers
		// The following four lines must match the previous four
		em_nupid,	// Number of project-scope identifiers
		em_nufid,	// Number of file-scope (static) identifiers
		em_numid,	// Number of macro identifiers
		em_nuid,	// Number of unique object and object-like identifiers
		em_nlabid,	// (INT) Number of label identifiers
		// The following metrics are dynamically derived
		em_nlabel,	// Number of goto labels (label identifiers - goto statements)
		metric_max,
		em_invalid = -1,
	};

	// Queue an identifier token for processing its constituent
	// parts when the function/file is processed
	void queue_identifier(const Ctoken &t) {
		queued_identifiers.emplace_back(t);
	}

	// Called for all file characters appart from identifiers
	void process_char(char c);
	// Called for every identifier
	void process_identifier(int len, Eclass *ec);
	void process_identifier(const string &s, Eclass *ec) {
		process_identifier(s.length(), ec);
	}
	// Summarize the identifiers collected by process_idendifier
	void summarize_identifiers();
	// Called when encountering unprocessed lines
	void add_unprocessed() { count[em_nuline]++; }

	// Manipulate the processing-based metrics
	void add_ppdirective() { if (!processed) count[em_nppdirective]++; }
	void add_ppcond() { if (!processed) count[em_nppcond]++; }
	void add_ppfmacro() { if (!processed) count[em_nppfmacro]++; }
	void add_ppomacro() { if (!processed) count[em_nppomacro]++; }
	void add_token() { if (!processed) count[em_ntoken]++; }

	void done_processing() { processed = true; }
	bool is_processed() const { return processed; }

	// Get methods
	enum e_cfile_state get_state() { return cstate; }
	// Generic
	// Return metric i (by lookup or calculation)
	double get_metric(int n) const;
	void set_metric(int n, int val) { count[n] = val; }
	void summarize_operators();

	// Update the maxumum level of statement nesting
	void update_nesting(int nesting) {
		if (nesting > count[em_maxstmtnest])
			count[em_maxstmtnest] = nesting;
	}

	// Call the specified metrics function for the current file and function
	// before or after the C preprocessor processing
	static void call_pre_cpp_metrics(void (Metrics::*fun)());
	static void call_post_cpp_metrics(void (Metrics::*fun)());

	// Return true if the specified metric shall not appear in the UI/RDBMS
	template <class M>
	static bool is_internal(int n) {
		return !is_pre_cpp<M>(n) && !is_post_cpp<M>(n) ;
	}

	// Return a reference to the details of the specified metric of class M
	template <class M>
	static const MetricDetails & get_metric_details(int n) {
		return M::metric_details[n];
	}

	// Return the database field name of the specified metric
	template <class M> static const string get_dbfield(int n);

	// Return true if the metric is applicable pre-cpp
	template <class M> static bool is_pre_cpp(int n);
	// Return true if the metric is applicable post-cpp
	template <class M> static bool is_post_cpp(int n);
	// Return true if the metric is applicable to files
	template <class M> static bool is_file(int n);

	// Return the metric name of the specified metric
	template <class M> static const string get_name(int n);

	/*
	 * Process a single token read from a file.
	 * This is templated, so that it can be called with
	 * Pltoken before the preprocessor and Ctoken after the preprocessor
	 * The metric_code is the return value of keyword_metric.
	 */
	template <typename TokenType> void process_token(const TokenType &t,
			Metrics::e_metric metric_code);

};

class KeywordMetrics {
private:
	// String-indexed enum metric map of keywords we collect
	typedef map<string, Metrics::e_metric> map_type;

	// Map from keywords to a metric that tallies them
	static map_type map;
	// Initialize keyword to code map
	static map_type make_keyword_map();
public:
	/* If a string represents a keyword we collect,
	 * return its metric enum value otherwise return -1
	 * Calling it before process_token allows us to amortize its cost over
	 * multiple calls.
	 */
	static inline Metrics::e_metric metric(const string &s) {
		auto i = map.find(s);
		return (i == map.end() ? Metrics::em_invalid : i->second);
	}
};

template <typename TokenType> void
Metrics::process_token(const TokenType &t, Metrics::e_metric metric_code)
{
	csassert(!processed);
	++currstmtlen;
	int code = t.get_code();
	switch (code) {
	case IDENTIFIER:
		if (metric_code != Metrics::em_invalid)
			count[metric_code]++;
		switch (metric_code) {
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
		default:
			break;
		}
		break;
	case ';':
		if (currstmtlen > count[em_maxstmtlen])
			count[em_maxstmtlen] = currstmtlen;
		currstmtlen = 0;
		count[em_nstmt]++;
		break;
	case PP_NUMBER:
		count[em_nnconst]++;
		break;
	case CHAR_LITERAL:
		count[em_nclit]++;
		break;
	case STRING_LITERAL:
		count[em_nstring]++;
		break;
	case '{':
		++currbracenesting;
		if (currbracenesting > count[em_maxbracenest])
			count[em_maxbracenest] = currbracenesting;
		break;
	case '}':
		--currbracenesting;
		break;
	case '(':
		++currbracknesting;
		if (currbracknesting > count[em_maxbracknest])
			count[em_maxbracknest] = currbracknesting;
		break;
	case ')':
		--currbracknesting;
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


// Return the is_pre_cpp field name of the specified metric
// i.e. if the metric is applicable to pre-cpp values
template <class M>
bool
Metrics::is_pre_cpp(int n)
{
	return Metrics::get_metric_details<M>(n).is_pre_cpp();
}

// Return the is_post_cpp field name of the specified metric
// i.e. if the metric is applicable to post-cpp values
template <class M>
bool
Metrics::is_post_cpp(int n)
{
	return Metrics::get_metric_details<M>(n).is_post_cpp();
}

// Return the is_file field name of the specified metric
// i.e. if the metric is applicable to files
template <class M>
bool
Metrics::is_file(int n)
{
	return Metrics::get_metric_details<M>(n).is_file();
}

// Return the database field name of the specified metric
template <class M>
const string
Metrics::get_dbfield(int n)
{
	return Metrics::get_metric_details<M>(n).get_dbfield();
}

// Return the metric name of the specified metric
template <class M>
const string
Metrics::get_name(int n)
{
	return Metrics::get_metric_details<M>(n).get_name();
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

	// Update metrics summary
	template <class BinaryFunction>
	void add(E &fi, BinaryFunction f) {
		nelement++;
		for (int i = 0; i < M::metric_max; i++) {
			count[i] = f(fi.get_pre_cpp_metrics().get_metric(i), count[i]);
			count[i] = f(fi.get_post_cpp_metrics().get_metric(i), count[i]);
		}
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
