/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * Encapsulates a (user interface) function query
 *
 */

#ifndef FUNQUERY_
#define FUNQUERY_

#include <string>

using namespace std;

#include "query.h"
#include "mquery.h"
#include "fileid.h"
#include "call.h"

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
	bool exclude_fre;	// Filename RE
	// Query arguments
	char match_type;	// Type of boolean match
	bool cfun;		// True if C function
	bool macro;		// True if function-like macro
	bool writable;		// True if writable
	bool ro;		// True if read-only
	bool pscope;		// True if project scoped
	bool fscope;		// True if file scoped
	bool defined;		// True if a definition was found
	Fileid fid;		// Match from this file
	bool match_fid;		// True to use the above
	int ncallers;		// Number of callers
	int ncallerop;		// Operator for comparing them

	Eclass *id_ec;		// True if identifier EC matches
				// No other evaluation takes place

	Call *call;		// True if call matches
				// No other evaluation takes place

	string name;		// Query name
	Attributes::size_type current_project;	// Restrict evaluation to this project
	// The query part for the metrics
	MQuery<FunMetrics, Call &> mquery;
public:
	// Construct object based on URL parameters
	FunQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Default
	FunQuery() : Query(), match_fnre(false), match_fdre(false), match_fure(false), match_fre(false), match_fid(false)  {}

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
	class specified_order {
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
				val = Query::string_bi_compare(a->get_name(), b->get_name());
			else
				val = (a->get_pre_cpp_const_metrics().get_metric(order) < b->get_pre_cpp_const_metrics().get_metric(order));
			return reverse ? !val : val;
		}
	};
	int get_sort_order() const { return mquery.get_sort_order(); }
	// Return true if the query's URL can be bookmarked across CScout invocations
	bool bookmarkable() const { return id_ec == NULL; }
};

typedef multiset <const Call *, FunQuery::specified_order> Sfuns;

#endif // FUNQUERY_
