/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A C token.
 * The getnext() method for these tokens converts preprocessor tokens to C
 * tokens.
 *
 * $Id: ctoken.h,v 1.3 2001/09/10 12:17:32 dds Exp $
 */

#ifndef CTOKEN_
#define CTOKEN_

char unescape_char(const string& s, string::const_iterator& si);

typedef map<string, int> mapKeyword;

class Ctoken: public Token {
private:
	static mapKeyword keywords;
	static mapKeyword& create_keymap();
public:
	Ctoken(Pdtoken& t) :
		Token(t) {};
	static int lookup_keyword(const string& s);
	void getnext();
};

int parse_lex();

#endif // CTOKEN
