/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Encapsulates an (user interface) identifier query
 * Can be used to evaluate against IdProp elements
 *
 * $Id: idquery.h,v 1.10 2006/09/24 20:58:46 dds Exp $
 */

#ifndef IDQUERY_
#define IDQUERY_

// Our identifiers to store as a map
class Identifier {
	string id;		// Identifier name
	string newid;		// New identifier name
	bool xfile;		// True if it crosses files
	bool replaced;		// True if newid has been set
	bool active;		// True if the replacement is active
public:
	Identifier(Eclass *e, const string &s) : id(s), replaced(false), active(true) {
		/*
		 * Normally, e crosses a file boundary if the EFn, the # of files it appears in > 1
		 * If we take into account the possibility of identical files EFi we must also
		 * ensure that EFn != EFi
		 * For this second condition it is enough to check the identical files of one element
		 */
		Tokid amember(*(e->get_members().begin()));
		xfile = e->sorted_files().size() > amember.get_fileid().get_identical_files().size();
	}
	Identifier() {}
	string get_id() const { return id; }
	void set_newid(const string &s) { newid = s; replaced = true; }
	string get_newid() const { return newid; }
	bool get_xfile() const { return xfile; }
	bool get_replaced() const { return replaced; }
	bool get_active() const { return active; }
	void set_xfile(bool v) { xfile = v; }
	void set_active(bool v) { active = v; }
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
	CompiledRE fre, ire;	// Compiled REs
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
	IdQuery::IdQuery() : Query(), match_fre(false), match_ire(false) {}


	// Destructor
	virtual ~IdQuery() {}

	// Perform a query
	bool eval(const IdPropElem &i);
	// Return the URL for re-executing this query
	string base_url() const;
	// Return the query's parameters as a URL
	string param_url() const;
	static void usage();	// Report string constructor usage information
};

/*
 * Function object to compare IdProp identifier pointers
 * Will compare from end to start if sort_rev is set
 */
struct idcmp : public binary_function <const IdProp::value_type *, const IdProp::value_type *, bool> {
	bool operator()(const IdProp::value_type *i1, const IdProp::value_type *i2) const
	{
		if (Option::sort_rev->get()) {
			const string &s1 = (*i1).second.get_id();
			const string &s2 = (*i2).second.get_id();
			string::const_reverse_iterator j1, j2;

			for (j1 = s1.rbegin(), j2 = s2.rbegin();
			     j1 != s1.rend() && j2 != s2.rend(); j1++, j2++)
				if (*j1 != *j2)
					return *j1 < *j2;
			return j1 == s1.rend() && j2 != s2.rend();
		} else
			return (*i1).second.get_id().compare((*i2).second.get_id()) < 0;
	}
};

typedef multiset <const IdProp::value_type *, idcmp> Sids;

#endif // IDQUERY_
