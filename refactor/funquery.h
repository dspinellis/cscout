/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates a (user interface) function query
 *
 * $Id: funquery.h,v 1.1 2004/07/25 14:46:35 dds Exp $
 */

#ifndef FUNQUERY_
#define FUNQUERY_

class FunQuery : public Query {
private:
	char match_type;	// Type of boolean match
	// Regular expression match specs
	string str_fre, str_ire;// Original REs
	regex_t fre, ire;	// Compiled REs
	bool match_fre, match_ire;
	bool exclude_ire;	// Exclude matched identifiers
	// Attribute match specs
	vector <bool> match;
	// Other query arguments
	bool xfile;		// True if cross file
	bool unused;		// True if unused id (EC size == 1 and not declared unused)
	bool writable;		// True if writable
	Eclass *ec;		// True if identifier EC matches
				// No other evaluation takes place
	string name;		// Query name
	Attributes::size_type current_project;	// Restrict evaluation to this project
public:
	// Construct object based on URL parameters
	FunQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Construct object based on a string specification
	FunQuery::FunQuery(const string &s);
	// Default
	FunQuery::FunQuery() : Query() {}

	// Destructor
	~FunQuery() {
		if (match_ire)
			regfree(&ire);
		if (match_fre)
			regfree(&fre);
	}

	// Perform a query
	bool eval(const IdPropElem &i);
	// Transform the query back into a URL
	string url();
	static void usage();	// Report string constructor usage information
};


#endif // FUNQUERY_
