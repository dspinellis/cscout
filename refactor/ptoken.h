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
 * $Id: ptoken.h,v 1.2 2001/08/18 21:43:35 dds Exp $
 */

#ifndef PTOKEN_
#define PTOKEN_

class Ptoken : public Token {
protected:
	string val;		// Token contents
public:
	// Construct it based on the token code and the contents
	Ptoken(int icode, const string& ival) : Token(icode), val(ival) {};
	// Accessor methods
	inline const string& get_val() const { return (val); };
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Ptoken &t);
};

#endif /* PTOKEN_ */
