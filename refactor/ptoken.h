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
 * $Id: ptoken.h,v 1.11 2006/07/29 07:26:35 dds Exp $
 */

#ifndef PTOKEN_
#define PTOKEN_

typedef set <Token> HideSet;

class Ptoken : public Token {
private:
	HideSet hideset;	// Hide set used for macro expansions
public:
	// Construct it based on the token code and the contents
	Ptoken(int icode, const string& ival) : Token(icode, ival) {};
	// Efficient constructor
	Ptoken() {};
	// Accessor methods
	inline bool hideset_contains(Ptoken t) const { return (hideset.find(t) != hideset.end()); }
	inline void hideset_insert(Token t) { hideset.insert(t); }
	inline void hideset_insert(HideSet::const_iterator b, HideSet::const_iterator e) { hideset.insert(b, e); }
	inline const HideSet& get_hideset() const { return (hideset); }
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Ptoken &t);
	inline friend bool operator ==(const Ptoken& a, const Ptoken& b);
	// Return true if \n or SPACE
	inline bool is_space() const;
};

typedef list<Ptoken> PtokenSequence;

ostream& operator<<(ostream& o,const PtokenSequence &t);

bool operator ==(const Ptoken& a, const Ptoken& b)
{
	return (a.val == b.val);
}

inline bool Ptoken::is_space() const
{
	return (code == SPACE || code == '\n');
}
#endif /* PTOKEN_ */
