/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * A metrics and a metrics summary container.
 *
 * One metrics object is created and updated for every function.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing
 * overhead will be incured exactly once for each file.
 *
 * During postprocessing call:
 * process_char() or process_id() while going through each file
 *
 * $Id: funmetrics.h,v 1.1 2007/08/13 15:09:49 dds Exp $
 */

#ifndef FUNMETRICS_
#define FUNMETRICS_


class Call;

class FunctionMetrics : public Metrics {
private:
	static MetricDetails metric_details[];
	Call *call;			// Associated function

public:
	FunctionMetrics(Call *c) : call(c) { count.resize(stored_metric_max, 0); }

	/*
	 * Metrics we collect or calculate
	 * Metrics marked with (INT) are not visible to the user,
	 * but are used internally to derive other user-visible
	 * metrics.
	 */
	enum e_metric {
		// Elements counted before the preprocessor token tap
		em_nsemi =	// Number of statements or declarations
			Metrics::metric_max,
		em_nop,		// Number of operators
		em_nuop,	// Number of unique operators
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
		em_npid,	// Number of project-scope identifiers
		em_nfid,	// Number of file-scope (static) identifiers
		em_nmid,	// Number of macro identifiers
		em_nid,		// Total number of object and object-like identifiers
		em_nuid,	// Number of unique object and object-like identifiers
		em_nlabid,	// (INT) Number of label identifiers
		// During processing (once based on processed)
		em_nparam,	// Number of parameters
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
	virtual ~FunctionMetrics() {}

	template <class M> friend const struct MetricDetails &get_detail(int n);
};

#endif /* FUNMETRICS_ */
