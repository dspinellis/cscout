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
 * $Id: ptoken.h,v 1.8 2001/09/08 18:12:48 dds Exp $
 */

#ifndef PTOKEN_
#define PTOKEN_


class Ptoken : public Token {
private:
	bool nonreplaced;	// True if replacement skipped during macro
				// expansions ANSI 3.8.3.4 p. 92
protected:
	string val;		// Token contents
public:
	// Construct it based on the token code and the contents
	Ptoken(int icode, const string& ival) : Token(icode), val(ival) {nonreplaced = false; };
	// Efficient constructor
	Ptoken() {nonreplaced = false; };
	// Accessor methods
	inline const string& get_val() const { return (val); };
	inline bool can_replace() const { return !nonreplaced; };
	inline void set_nonreplaced() { nonreplaced = true; };
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Ptoken &t);
	inline friend bool operator ==(const Ptoken& a, const Ptoken& b);
	// Return true if \n or SPACE
	inline bool is_space() const;
};

typedef list<Ptoken> listPtoken;

ostream& operator<<(ostream& o,const listPtoken &t);

bool operator ==(const Ptoken& a, const Ptoken& b)
{
	return (a.val == b.val);
}

inline bool Ptoken::is_space() const
{
	return (code == SPACE || code == '\n');
}
#endif /* PTOKEN_ */
