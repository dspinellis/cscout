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
 * $Id: token.h,v 1.3 2001/08/18 21:56:31 dds Exp $
 */

#ifndef TOKEN_
#define TOKEN_

class Token {
protected:
	int code;			// Token type code
	dequeTokid parts;		// Identifiers for constituent parts
public:
	Token(int icode) : code(icode) {};
	Token() {};
	// Accessor method
	inline int get_code() const { return (code); }
	// Return the token's symbolic name based on its code
	string name() const;
	// Send it on ostream
	friend ostream& operator<<(ostream& o,const Token &t);
};

#endif /* TOKEN_ */
