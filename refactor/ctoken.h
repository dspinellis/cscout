/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A C token.
 * The getnext() method for these tokens converts preprocessor tokens to C
 * tokens.
 *
 * $Id: ctoken.h,v 1.2 2001/09/08 17:31:14 dds Exp $
 */

#ifndef CTOKEN_
#define CTOKEN_

char unescape_char(const string& s, string::const_iterator& si);

typedef map<string, int> mapKeyword;

class Ctoken: public Token {
private:
	static mapKeyword keywords;
	static mapKeyword& create_keymap();
	string name;				// For identifiers
public:
	Ctoken(string& n, Token t) :
		Token(t), name(n) {};
	static int lookup_keyword(const string& s);
	const string& get_name() const { return (name); };
	void getnext();
};

#endif // CTOKEN
