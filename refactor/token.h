/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A lexical token.
 *
 * Include synopsis:
 * #include <iostream>
 * #include <deque>
 * #include "tokid.h"
 *
 * $Id: token.h,v 1.7 2001/10/27 06:51:00 dds Exp $
 */

#ifndef TOKEN_
#define TOKEN_

// A token part; the smallest unit that is recognised for replacement
class Tpart {
private:
	Tokid ti;		// Token part identifier
	int len;		// Its length
public:
	Tpart() {};
	Tpart(Tokid t, int l): len(l), ti(t) {};
	Tokid get_tokid() const { return ti; }
	int get_len() const { return len; }
	friend ostream& operator<<(ostream& o, const Tpart &t);
};

typedef deque<Tpart> dequeTpart;
// Print dequeTpart sequences
ostream& operator<<(ostream& o,const dequeTpart& dt);

class Token {
protected:
	int code;			// Token type code
	dequeTpart parts;		// Identifiers for constituent parts
protected:
	string val;		// Token character contents (for identifiers)
public:
	Token(int icode) : code(icode) {};
	Token(int icode, const string& v) 
		: code(icode), val(v) {};
	Token() {};
	// Accessor method
	int get_code() const { return (code); }
	const string& get_val() const { return (val); };
	const string& get_name() const { return (val); };
	// Return the token's symbolic name based on its code
	string name() const;
	// Return the constituent Tokids; they may be more than the parts
	dequeTpart Token::constituents() const;
	// Send it on ostream
	friend ostream& operator<<(ostream& o,const Token &t);
	// Iterators for accessing the token parts
	inline dequeTpart::const_iterator get_parts_begin() const;
	inline dequeTpart::const_iterator get_parts_end() const;
};

/* Given two Tokid sequences corresponding to two tokens
 * make these correspond to equivalence classes of same lengths.
 * Getting the Token constituents again will return Tokids that
 * satisfy the above postcondition.
 * The operation only modifies the underlying equivalence classes
 */
void homogenize(const dequeTokid &a, const dequeTokid &b);

// Unify the constituent equivalence classes for a and b
void unify(const Token &a, const Token &b);

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

#endif /* TOKEN_ */
