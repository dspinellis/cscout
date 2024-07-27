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
 * A C token.
 * The getnext() method for these tokens converts preprocessor tokens to C
 * tokens.
 *
 */

#ifndef CTOKEN_
#define CTOKEN_

#include <string>
#include <map>

#include "token.h"
#include "pdtoken.h"

using namespace std;

char unescape_char(const string& s, string::const_iterator& si);

typedef map<string, int> mapKeyword;

class Ctoken: public Token {
private:
	static mapKeyword keywords;
	static mapKeyword& create_keymap();
public:
	Ctoken() {}
	Ctoken(Pdtoken& t) :
		Token(t) {};
	/*
	 * Return the string's keyword token value or -1 if the string
	 * isn't a keyword.
	 */
	static int lookup_keyword(const string& s);
	void getnext();
};

int parse_lex();

#endif // CTOKEN
