/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A preprocessor token.
 *
 *
 */

#ifndef PTOKEN_
#define PTOKEN_

#include <iostream>
#include <map>
#include <string>
#include <deque>

using namespace std;

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"

class Ctoken;

typedef set <Token> HideSet;

class Ptoken : public Token {
private:
	HideSet hideset;	// Hide set used for macro expansions
public:
	// Construct it based on the token code and the contents
	Ptoken(int icode, const string& ival) : Token(icode, ival) {};
	// Efficient constructor
	Ptoken() {}
	// Construct it from a CToken
	Ptoken(const Ctoken &t);
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
