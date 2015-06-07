/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
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
