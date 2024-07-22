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
		// During processing (once based on processed)
		em_ngnsoc =	// Number of global namespace occupants at function's top
			Metrics::metric_max,
		em_nmparam,	// Number of macro parameters
		em_nfparam,	// Number of function parameters
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

	// Summarize the operators collected by process_token
	void summarize_operators();
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
