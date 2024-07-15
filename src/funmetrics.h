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
 */

#ifndef FUNMETRICS_
#define FUNMETRICS_

#include <set>

using namespace std;

#include "metrics.h"
#include "parse.tab.h"

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

	/*
	 * Process a single token read from a file.
	 * This is templated, so that it can be called with
	 * Pltoken before the preprocessor and Ctoken after the preprocessor
	 */
	template <typename TokenType> void process_token(const TokenType &t);

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


template <typename TokenType> void
FunMetrics::process_token(const TokenType &t)
{
	csassert(!processed);
	int code = t.get_code();
	int em;
	switch (code) {
	case IDENTIFIER:
		em = keyword_metric(t.get_val());
		if (em != -1)
			count[em]++;
		switch (em) {
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
		}
		break;
	case ';':
		count[em_nstmt]++;
		break;
	case PP_NUMBER:
		count[em_nnconst]++;
		break;
	case CHAR_LITERAL:
		count[em_nclit]++;
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
