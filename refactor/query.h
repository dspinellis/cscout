/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates the common parts of a (user interface) query
 *
 * $Id: query.h,v 1.1 2004/07/25 14:46:35 dds Exp $
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
public:
	static string url(const string &s);
	// Accessor functions
	bool is_valid() { return valid; }
	bool need_eval() { return !lazy; }
};


#endif // QUERY_
