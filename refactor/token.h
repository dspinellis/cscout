/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A lexical token.
 *
 *
 */

#ifndef TOKEN_
#define TOKEN_

#include <iostream>
#include <deque>
#include <string>

#include "tokid.h"

using namespace std;

// A token part; the smallest unit that is recognised for replacement
class Tpart {
private:
	Tokid ti;		// Token part identifier
	int len;		// Its length
public:
	Tpart() {};
	Tpart(Tokid t, int l): ti(t), len(l) {};
	static void homogenize(const dequeTpart &a, const dequeTpart &b);
	Tokid get_tokid() const { return ti; }
	int get_len() const { return len; }
	friend ostream& operator<<(ostream& o, const Tpart &t);
	inline friend bool operator ==(const class Tpart &a, const class Tpart &b);
	inline friend bool operator !=(const class Tpart &a, const class Tpart &b);
	inline friend bool operator <(const class Tpart &a, const class Tpart &b);
};

typedef deque<Tpart> dequeTpart;
// Print dequeTpart sequences
ostream& operator<<(ostream& o,const dequeTpart& dt);

class Token {
protected:
	int code;			// Token type code
	dequeTpart parts;		// Identifiers for constituent parts
	string val;			// Token character contents (for identifiers)
public:
	// Modify class's operation to check for name clashes of refactored ids
	static bool check_clashes;
	// True if during checks identifier clashes were detected
	static bool found_clashes;

	// Unify the constituent equivalence classes for def and ref
	// The definition/reference order is only required when maintaining
	// dependency relationships across files
	static void unify(const Token &def, const Token &ref);

	Token(int icode) : code(icode) {};
	Token(int icode, const string& v)
		: code(icode), val(v)
	{
		parts.push_front(Tpart(Tokid(0, 0), v.length()));
	}
	Token() {};
	// Accessor method
	int get_code() const { return (code); }
	// Return an identifier token's name
	const string get_name() const { return check_clashes ? get_refactored_name() : val; };
	const string get_val() const { return get_name(); };
	// Return the value escaping strings as needed
	const string get_c_val() const;
	// Return the name after applying rename identifier refactorings
	const string get_refactored_name() const;
	// Return the token's symbolic name based on its code
	string name() const;
	// Return the constituent Tokids; they may be more than the parts
	dequeTpart constituents() const;
	// Return a token that uniquely represents all same tokens coming from identical files
	Token unique() const;
	// Return the Tokid best defining this token wrt the current file position
	Tokid get_defining_tokid() const;
	// Send it on ostream
	friend ostream& operator<<(ostream& o,const Token &t);
	// Iterators for accessing the token parts
	inline dequeTpart::const_iterator get_parts_begin() const;
	inline dequeTpart::const_iterator get_parts_end() const;
	inline dequeTpart::size_type get_parts_size() const {
		return parts.size();
	}
	// Set the token's equivalence class attribute
	void set_ec_attribute(enum e_attribute a) const;
	// Return true if the token's equivalence class(es) contain the attribute
	bool has_ec_attribute(enum e_attribute a) const;
	// Return true if the token contains in its parts the given EC
	bool contains(Eclass *ec) const;
	// Return true if its tokids equal those of stale
	bool equals(const Token &stale) const;
	// For including them in sets
	inline friend bool operator ==(const class Token &a, const class Token &b);
	inline friend bool operator !=(const class Token &a, const class Token &b);
	inline friend bool operator <(const class Token &a, const class Token &b);
};

dequeTpart::const_iterator
Token::get_parts_begin() const
{
	return parts.begin();
}

dequeTpart::const_iterator
Token::get_parts_end() const
{
	return parts.end();
}
inline bool
operator ==(const class Tpart &a, const class Tpart &b)
{
	return (a.ti == b.ti && a.len == b.len);
}

inline bool
operator !=(const class Tpart &a, const class Tpart &b)
{
	return (!(a == b));
}

inline bool
operator <(const class Tpart &a, const class Tpart &b)
{
	return ((a.ti == b.ti) ? (a.len < b.len) : (a.ti < b.ti));
}

inline bool
operator ==(const class Token &a, const class Token &b)
{
	if (a.parts.size() != b.parts.size())
		return (false);
	dequeTpart::const_iterator ia, ib;
	for (ia = a.parts.begin(), ib = b.parts.begin(); ia != a.parts.end(); ia++, ib++)
		if (*ia != *ib)
			return (false);
	return (true);
}

inline bool
operator !=(const class Token &a, const class Token &b)
{
	return (!(a == b));
}

inline bool
operator <(const class Token &a, const class Token &b)
{
	return (lexicographical_compare(a.parts.begin(), a.parts.end(), b.parts.begin(), b.parts.end()));
}

#endif /* TOKEN_ */
