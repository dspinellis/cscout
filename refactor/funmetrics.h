/*
 * (C) Copyright 2002-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A metrics and a metrics summary container.
 *
 * One metrics object is created and updated for every function.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing
 * overhead will be incured exactly once for each file.
 *
 * Before preprocessing call:
 * process_token(int code) for every token recognized
 * During postprocessing call:
 * process_char() or process_id() while going through each file
 * summarize_identifiers() at the end of each function
 *
 * $Id: funmetrics.h,v 1.16 2009/01/15 14:32:57 dds Exp $
 */

#ifndef FUNMETRICS_
#define FUNMETRICS_

#include <set>

using namespace std;

#include "metrics.h"
#include "ytab.h"

class Call;
class Pltoken;
class Eclass;

class FunMetrics : public Metrics {
private:
	static MetricDetails metric_details[];	// Descriptions of the metrics we store

	// Int-indexed map of tokens that are operators
	static vector<bool> &is_operator_map;
	// Return true if token op is an operator
	static inline bool is_operator(unsigned op) { return op < is_operator_map.size() && is_operator_map[op]; }
	// Add an operator to the map
	static inline void add_operator(vector<bool> &v, unsigned op);
	// Initialize map
	static vector<bool> &make_is_operator();

	// String-indexed enum metric map of keywords we collect
	typedef map<string, int> KeywordMap;
	static KeywordMap &keyword_map;
	// If a string represents a keyword we collect,
	// return its metric enum value otherwise return -1
	static inline int keyword_metric(const string &s) {
		KeywordMap::iterator i = keyword_map.find(s);
		return (i == keyword_map.end() ? -1 : i->second);
	}
	// Initialize map
	static KeywordMap &make_keyword_map();

	set <int> operators;			// Operators used in the function
	set <Eclass *> pids;			// Project-scope dentifiers used in the function
	set <Eclass *> fids;			// File-scope identifiers used in the function
	set <Eclass *> mids;			// Macro identifiers used in the function
	set <Eclass *> ids;			// Identifiers used in the function
	Call *call;				// Associated function

public:
	FunMetrics(Call *c) : call(c) { count.resize(stored_metric_max, 0); }

	/*
	 * Metrics we collect or calculate
	 * Metrics marked with (INT) are not visible to the user,
	 * but are used internally to derive other user-visible
	 * metrics.
	 */
	enum e_metric {
		// Elements counted before the preprocessor token tap
		em_nstmt =	// Number of statements or declarations
			Metrics::metric_max,
		em_nop,		// Number of operators
		em_nuop,	// Number of unique operators
		em_nnconst,	// Number of numeric constants
		em_nclit,	// Number of character literals
		em_ncc2op,	// (INT) Number of operators contributing to cc2: &&, ||, ?:
		// Keywords counted during identifier processing
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
		// Identifiers categorized during identifier processing
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
		// During processing (once based on processed)
		em_ngnsoc,	// Number of global namespace occupants at function's top
		em_nparam,	// Number of parameters
		em_maxnest,	// Maximum level of statement nesting
		// Stored metrics stop here
		stored_metric_max,
		// The following metrics are dynamically derived
		em_nlabel =	// Number of goto labels (label identifiers - goto statements)
			stored_metric_max,
		em_fanin,	// Fan-in (number of calling functions)
		em_fanout,	// Fan-out (number of called functions)
		em_ccycl1,	// Cyclomatic complexity (control statements)
		em_ccycl2,	// Extended cyclomatic complexity (includes branching operators)
		em_ccycl3,	// Maximum cyclomatic complexity (includes branching operators and all switch branches)
		// Metrics after this point are hardcoded to be stored in the database as REAL
		em_cstruc,	// Structure complexity (Henry and Kafura)
		em_real_start = em_cstruc,
		em_chal,	// Halstead complexity
		em_iflow,	// Information flow metric
		metric_max,
	};

	// Return metric i (by lookup or calculation)
	virtual double get_metric(int i) const;
	virtual ~FunMetrics() {}

	// Process a single token read from a file
	void process_token(const Pltoken &t);
	// Called for every identifier (override Metrics method)
	void process_id(const string &s, Eclass *ec);
	// Summarize the operators collected by process_token
	void summarize_operators();
	// Summarize the identifiers collected by process_id
	void summarize_identifiers();
	// Update the level of nesting
	void update_nesting(int nesting) { if (nesting > count[em_maxnest]) count[em_maxnest] = nesting; }

	template <class M> friend const MetricDetails &Metrics::get_detail(int n);
};

// This can be kept per project and globally
class FunMetricsSummary {
	MetricsRange<FunMetrics, Call> val;			// For read-only and writable cases
public:
	// Create function summary
	void summarize_functions();
	friend ostream& operator<<(ostream& o,const FunMetricsSummary &ms);
};

extern FunMetricsSummary fun_msum;

#endif /* FUNMETRICS_ */
