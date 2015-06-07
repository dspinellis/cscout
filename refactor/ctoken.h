/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
	static int lookup_keyword(const string& s);
	void getnext();
};

int parse_lex();

#endif // CTOKEN
