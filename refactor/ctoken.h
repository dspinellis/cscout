/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A C token.
 * The getnext() method for these tokens converts preprocessor tokens to C
 * tokens.
 *
 * $Id: ctoken.h,v 1.6 2008/12/04 15:19:06 dds Exp $
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
