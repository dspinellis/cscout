/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates a (user interface) function query
 *
 * $Id: funquery.h,v 1.11 2007/08/14 21:59:39 dds Exp $
 */

#ifndef FUNQUERY_
#define FUNQUERY_

class FunQuery : public Query {
private:
	// Regular expression match specs
	string str_fnre;	// Function name RE
	string str_fdre;	// Called function name RE (down)
	string str_fure;	// Calling function name RE (up)
	string str_fre;		// Filename RE
	// Compiled REs
	CompiledRE fnre;	// Function name RE
	CompiledRE fdre;	// Called function name RE (down)
	CompiledRE fure;	// Calling function name RE (up)
	CompiledRE fre;		// Filename RE
	// Match rules
	bool match_fnre;	// Function name RE
	bool match_fdre;	// Called function name RE (down)
	bool match_fure;	// Calling function name RE (up)
	bool match_fre;		// Filename RE
	// Exclude options
	bool exclude_fnre;	// Function name RE
	bool exclude_fdre;	// Called function name RE (down)
	bool exclude_fure;	// Calling function name RE (up)
	// Query arguments
	char match_type;	// Type of boolean match
	bool cfun;		// True if C function
	bool macro;		// True if function-like macro
	bool writable;		// True if writable
	bool ro;		// True if read-only
	bool pscope;		// True if project scoped
	bool fscope;		// True if file scoped
	bool defined;		// True if a definition was found
	int ncallers;		// Number of callers
	int ncallerop;		// Operator for comparing them

	Call *call;		// True if call matches
				// No other evaluation takes place

	string name;		// Query name
	Attributes::size_type current_project;	// Restrict evaluation to this project
	// The query part for the metrics
	MQuery<FunctionMetrics, Call &> mquery;
public:
	// Construct object based on URL parameters
	FunQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Default
	FunQuery() : Query(), match_fnre(false), match_fdre(false), match_fure(false), match_fre(false) {}

	// Destructor
	virtual ~FunQuery() {}

	// Perform a query
	bool eval(Call *c);
	// Return the URL for re-executing this query
	string base_url() const;
	// Return the query's parameters as a URL
	string param_url() const;
	//
	// Container comparison functor
	class specified_order : public binary_function <const Call &, const Call &, bool> {
	private:
		/*
		 * Can only be an instance variable (per C++ PL 17.1.4.5)
		 * only when the corresponding constructor is passed a
		 * compile-time constant.
		 * This hack works around the limitation.
		 */
		static int order;
		static bool reverse;
	public:
		// Should be called exactly once before instantiating the set
		static void set_order(int o, bool r) { order = o; reverse = r; }
		bool operator()(const Call *a, const Call *b) const {
			bool val;
			if (order == -1)
				// Order by name
				if (Option::sort_rev->get())
					val = Query::string_rev_compare(a->get_name(), b->get_name());
				else
					val = (a->get_name() < b->get_name());
			else
				val = (a->const_metrics().get_metric(order) < b->const_metrics().get_metric(order));
			return reverse ? !val : val;
		}
	};
};

typedef multiset <const Call *, FunQuery::specified_order> Sfuns;

#endif // FUNQUERY_
