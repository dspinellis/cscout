/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates a (user interface) function query
 *
 * $Id: funquery.h,v 1.6 2005/09/27 21:32:57 dds Exp $
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
	// Compiler REs
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
public:
	// Construct object based on URL parameters
	FunQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Default
	FunQuery::FunQuery() : Query(), match_fnre(false), match_fdre(false), match_fure(false), match_fre(false) {}

	// Destructor
	virtual ~FunQuery() {}

	// Perform a query
	bool eval(const Call *c);
	// Transform the query back into a URL
	virtual string url() const;
};

/*
 * Function object to compare Call pointers
 * Will compare from end to start if sort_rev is set
 */
struct fcmp : public binary_function <const Call *, const Call *, bool> {
	bool operator()(const Call *i1, const Call *i2) const
	{
		if (Query::sort_rev) {
			const string &s1 = i1->get_name();
			const string &s2 = i2->get_name();
			string::const_reverse_iterator j1, j2;

			for (j1 = s1.rbegin(), j2 = s2.rbegin();
			     j1 != s1.rend() && j2 != s2.rend(); j1++, j2++)
				if (*j1 != *j2)
					return *j1 < *j2;
			return j1 == s1.rend() && j2 != s2.rend();
		} else
			return i1->get_name().compare(i2->get_name()) < 0;
	}
};

typedef multiset <const Call *, fcmp> Sfuns;

#endif // FUNQUERY_
