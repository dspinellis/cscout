/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates the common parts of a (user interface) query
 *
 * $Id: query.h,v 1.2 2004/07/27 11:14:28 dds Exp $
 */

#ifndef QUERY_
#define QUERY_

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
	bool compile_re(FILE *of, const char *name, const char *varname, regex_t &re, bool &match,  string &str, int compflags = 0);
public:
	static string url(const string &s);
	// Accessor functions
	bool is_valid() { return valid; }
	bool need_eval() { return !lazy; }
	static bool sort_rev;			// Reverse sort of identifier names
};


#endif // QUERY_
