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
 * $Id: token.h,v 1.1 2001/08/18 15:29:45 dds Exp $
 */

#ifndef TOKEN_
#define TOKEN_

class Token {
private:
	int code;			// Token type code
protected:
	dequeTokid parts;		// Identifiers for constituent parts
public:
	Token(int icode) : code(icode) {};
	string name() const;
	friend ostream& operator<<(ostream& o,const Token &t);
};

#endif /* TOKEN_ */
