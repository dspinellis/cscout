/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor token.
 *
 * Include synopsis:
 * #include <iostream>
 * #include <map>
 * #include <string>
 * #include <deque>
 * 
 * #include "cpp.h"
 * #include "fileid.h"
 * #include "tokid.h"
 * #include "token.h"
 *
 * $Id: ptoken.h,v 1.5 2001/08/24 14:18:24 dds Exp $
 */

#ifndef PTOKEN_
#define PTOKEN_


class Ptoken : public Token {
protected:
	string val;		// Token contents
public:
	// Construct it based on the token code and the contents
	Ptoken(int icode, const string& ival) : Token(icode), val(ival) {};
	// Efficient constructor
	Ptoken() {};
	// Accessor methods
	inline const string& get_val() const { return (val); };
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Ptoken &t);
	inline friend bool operator ==(const Ptoken& a, const Ptoken& b);
};

typedef list<Ptoken> listPtoken;

ostream& operator<<(ostream& o,const listPtoken &t);

bool operator ==(const Ptoken& a, const Ptoken& b)
{
	return (a.val == b.val);
}

#endif /* PTOKEN_ */
