/*
 * (C) Copyright 2003-2015 Diomidis Spinellis
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
 * Encapsulates an (user interface) identifier query
 * Can be used to evaluate against IdProp elements
 *
 */

#ifndef IDQUERY_
#define IDQUERY_

#include <string>

using namespace std;

#include "query.h"
#include "eclass.h"

class Identifier;

typedef map <Eclass *, Identifier> IdProp;

// Our identifiers to store as a map
class Identifier {
	string id;		// Identifier name
	string newid;		// New identifier name
	bool xfile;		// True if it crosses files
	bool replaced;		// True if newid has been set
	bool active;		// True if the replacement is active
public:
	// Additional identifier properties required for refactoring
	static IdProp ids;

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
	Identifier() : xfile(false), replaced(false), active(false) {}
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

typedef IdProp::value_type IdPropElem;

class IdQuery : public Query {
private:
	char match_type;	// Type of boolean match
	// Regular expression match specs
	string str_fre, str_ire;// Original REs
	CompiledRE fre, ire;	// Compiled REs
	bool match_fre, match_ire;
	bool exclude_ire;	// Exclude matched identifiers
	bool exclude_fre;	// Exclude matched files
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
	IdQuery(const string &s);
	// Default
	IdQuery() : Query(), match_fre(false), match_ire(false) {}


	// Destructor
	virtual ~IdQuery() {}

	// Perform a query
	bool eval(const IdPropElem &i);
	// Return the URL for re-executing this query
	string base_url() const;
	// Return the query's parameters as a URL
	string param_url() const;
	static void usage();	// Report string constructor usage information
	// Return true if the query's URL can be bookmarked across CScout invocations
	bool bookmarkable() const { return ec == NULL; }
};

/*
 * Function object to compare IdProp identifier pointers
 * Will compare from end to start if sort_rev is set
 */
struct idcmp : public binary_function <const IdProp::value_type *, const IdProp::value_type *, bool> {
	bool operator()(const IdProp::value_type *i1, const IdProp::value_type *i2) const
	{
		return Query::string_bi_compare(i1->second.get_id(), i2->second.get_id());
	}
};

typedef multiset <const IdProp::value_type *, idcmp> Sids;

#endif // IDQUERY_
