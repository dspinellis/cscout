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
 * $Id: token.h,v 1.4 2001/08/21 18:29:45 dds Exp $
 */

#ifndef TOKEN_
#define TOKEN_

// A token part
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
public:
	Token(int icode) : code(icode) {};
	Token() {};
	// Accessor method
	inline int get_code() const { return (code); }
	// Return the token's symbolic name based on its code
	string name() const;
	// Return the constituent Tokids; they may be more than the parts
	dequeTpart Token::constituents() const;
	// Send it on ostream
	friend ostream& operator<<(ostream& o,const Token &t);
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

#endif /* TOKEN_ */
