/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates an (user interface) identifier query
 * Can be used to evaluate against IdProp elements
 *
 * $Id: idquery.h,v 1.2 2004/07/25 14:47:53 dds Exp $
 */

#ifndef IDQUERY_
#define IDQUERY_

// Our identifiers to store as a map
class Identifier {
	string id;		// Identifier name
	string newid;		// New identifier name
	bool xfile;		// True if it crosses files
	bool replaced;		// True if newid has been set
public:
	Identifier(Eclass *e, const string &s) : id(s), replaced(false) {
		xfile = e->sorted_files().size() > 1;
	}
	Identifier() {}
	string get_id() const { return id; }
	void set_newid(const string &s) { newid = s; replaced = true; }
	string get_newid() const { return newid; }
	bool get_xfile() const { return xfile; }
	bool get_replaced() const { return replaced; }
	void set_xfile(bool v) { xfile = v; }
	// To create nicely ordered sets
	inline bool operator ==(const Identifier b) const {
		return (this->id == b.id);
	}
	inline bool operator <(const Identifier b) const {
		return this->id.compare(b.id);
	}
};

typedef map <Eclass *, Identifier> IdProp;
typedef IdProp::value_type IdPropElem;

class IdQuery : public Query {
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
	IdQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Construct object based on a string specification
	IdQuery::IdQuery(const string &s);
	// Default
	IdQuery::IdQuery() : Query() {}

	// Destructor
	~IdQuery() {
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


#endif // IDQUERY_
