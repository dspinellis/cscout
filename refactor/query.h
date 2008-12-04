/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates the common parts of a (user interface) query
 *
 * $Id: query.h,v 1.12 2008/12/04 15:19:06 dds Exp $
 */

#ifndef QUERY_
#define QUERY_

#include <iostream>

using namespace std;

#include "attr.h"
#include "option.h"
#include "compiledre.h"
#include "debug.h"

class Query {
protected:
	bool lazy;		// Do not evaluate
	bool return_val;	// Default return value
	bool valid;		// True if query is valid
	string name;		// Query name
	Attributes::size_type current_project;	// Restrict evaluation to this project
	Query() : valid(false) {}
	Query(bool l, bool r, bool v) : lazy(l), return_val(r), valid(v) {}
	/*
	 * Compile a regular expression name name, stored in SWILL varname
	 * into re.
	 * Set match to true if it was compiled and str to the RE string.
	 * Compflags can supply additional compilation flags.
	 * Return true if OK, false on error
	 * Errors are sent to of
	 */
	bool compile_re(FILE *of, const char *name, const char *varname, CompiledRE &re, bool &match,  string &str, int compflags = 0);
public:
	// Comparisson constants used for selections
	enum e_cmp {
		ec_ignore,
		ec_eq,
		ec_ne,
		ec_lt,
		ec_gt
	};
	// Display an equality selection box
	static void equality_selection(FILE *of);
	// Return the result of applying operator op on a, b
	static inline bool apply(int op, double a, double b);
	// URL-encode the given string
	static string url(const string &s);

	virtual ~Query() {}
	// Accessor functions
	bool is_valid() { return valid; }
	bool need_eval() { return !lazy; }
	// Return the URL for listing a file based on this query
	virtual string param_url() const = 0;
	// Return true if the query's URL can be bookmarked across CScout invocations
	virtual bool bookmarkable() const = 0;
	// Return the URL for re-executing this query
	virtual string base_url() const = 0;
	// Compare two strings from end to start or start to end
	static bool string_bi_compare(const string &s1, const string &s2) {
		if (Option::sort_rev->get()) {
			string::const_reverse_iterator j1, j2;

			for (j1 = s1.rbegin(), j2 = s2.rbegin();
			     j1 != s1.rend() && j2 != s2.rend(); j1++, j2++)
				if (*j1 != *j2)
					return *j1 < *j2;
			return j1 == s1.rend() && j2 != s2.rend();
		} else
			return s1.compare(s2) < 0;
	}
};

/*
 * Return the result of applying operator op on a, b
 * Apparently doing this with the standard equal_to, etc functors would require a
 * 5-template function.
 * Attempting to store all functors in a vector <binary_function <..> > is useless
 * since because the polymorphic binary_function does not define the appropriate () operators.
 */
inline bool
Query::apply(int op, double a, double b)
{
	if (DP()) {
		cout << a;
		switch (op) {
		case ec_eq: cout << " == "; break;
		case ec_ne: cout << " != "; break;
		case ec_lt: cout << " < "; break;
		case ec_gt: cout << " > "; break;
		}
		cout << b << "\n";
	}
	switch (op) {
	case ec_eq: return a == b;
	case ec_ne: return a != b;
	case ec_lt: return a < b;
	case ec_gt: return a > b;
	default: return false;
	}
}

#endif // QUERY_
