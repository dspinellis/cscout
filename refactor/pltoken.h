/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor lexical token.
 * The constructor for these tokens converts characters into tokens.
 *
 * Include synopsis:
 * #include <iostream>
 * #include <map>
 * #include <string>
 * #include <deque>
 * #include <stack>
 * #include <iterator>
 * #include <cassert>
 * 
 * #include "cpp.h"
 * #include "fileid.h"
 * #include "tokid.h"
 * #include "token.h"
 * #include "error.h"
 * #include "ptoken.h"
 * #include "fchar.h"
 * #include "ytab.h"
 *
 * $Id: pltoken.h,v 1.7 2001/08/20 17:05:40 dds Exp $
 */

#ifndef PLTOKEN_
#define PLTOKEN_

class Fchar;

// C-preprocessor context influences lexical analysis
enum e_cpp_context {cpp_normal, cpp_include, cpp_define};

class Pltoken: public Ptoken {
private:
	static enum e_cpp_context context;
public:
	template <class C> void getnext();
	static void set_context(enum e_cpp_context c) { context = c; };
};

/*
 * Construct a preprocessor lexical token using Fchar as the class to
 * provide character input.
 * Substitute comments with a single space.
 * Continous character sequences that can be associated with an identifier
 * letters and digits are given an equivalence class.
 *
 * Should probably be declared as export, but VC 5 can not handle it.
 */
template <class C>
void
Pltoken::getnext()
{
	int n;
	C c0, c1;

	c0.getnext();
	switch (c0.get_char()) {
	/* 
	 * Single character C operators and punctuators 
	 * ANSI 3.1.5 p. 32 and 3.1.6 p. 33
	 */
	case '\n':	// Needed for processing directives
		//incpp = false;
		// FALLTRHOUGH
	case '[': case ']': case '(': case ')': 
	case '~': case '?': case ':': case ',': case ';':
	case '{': case '}':
	case EOF:
		val = (char)(code = c0.get_char());
		break;
	/* 
	 * Double character C tokens with more than 2 different outcomes 
	 * (e.g. +, +=, ++)
	 */
	case '+':
		c0.getnext();
		switch (c0.get_char()) {
		case '+': val = "++"; code = INC_OP; break;
		case '=': val = "+="; code = ADD_ASSIGN; break;
		default:  C::putback(c0); val = (char)(code = '+'); break;
		}
		break;
	case '-':
		c0.getnext();
		switch (c0.get_char()) {
		case '-': val = "--"; code = DEC_OP; break;
		case '=': val = "-="; code = SUB_ASSIGN; break;
		case '>': val = "->"; code = PTR_OP; break;
		default:  C::putback(c0); val = (char)(code = '-'); break;
		}
		break;
	case '&':
		c0.getnext();
		switch (c0.get_char()) {
		case '&': val = "&&"; code = AND_OP; break;
		case '=': val = "&="; code = AND_ASSIGN; break;
		default:  C::putback(c0); val = (char)(code = '&'); break;
		}
		break;
	case '|':
		c0.getnext();
		switch (c0.get_char()) {
		case '|': val = "||"; code = OR_OP; break;
		case '=': val = "|="; code = OR_ASSIGN; break;
		default:  C::putback(c0); val = (char)(code = '|'); break;
		}
		break;
	/* Simple single/double character tokens (e.g. !, !=) */
	case '!':
		c0.getnext();
		if (c0.get_char() == '=') {
			val = "!=";
			code = NE_OP;
		} else {
			C::putback(c0);
			val = (char)(code = '!');
		}
		break;
	case '%':
		c0.getnext();
		if (c0.get_char() == '=') {
			val = "%=";
			code = MOD_ASSIGN;
		} else {
			C::putback(c0);
			val = (char)(code = '%');
		}
		break;
	case '*':
		c0.getnext();
		if (c0.get_char() == '=') {
			val = "*=";
			code = MUL_ASSIGN;
		} else {
			C::putback(c0);
			val = (char)(code = '*');
		}
		break;
	case '=':
		c0.getnext();
		if (c0.get_char() == '=') {
			val = "==";
			code = EQ_OP;
		} else {
			C::putback(c0);
			val = (char)(code = '=');
		}
		break;
	case '^':
		c0.getnext();
		if (c0.get_char() == '=') {
			val = "^=";
			code = XOR_ASSIGN;
		} else {
			C::putback(c0);
			val = (char)(code = '^');
		}
		break;
	case '#':	/* C-preprocessor token only */
		// incpp = true;		// Overkill, but good enough
		c0.getnext();
		if (c0.get_char() == '#') {
			val = "##";
			code = CPP_CONCAT;
		} else {
			C::putback(c0);
			val = (char)(code = '#');
		}
		break;
	/* Operators starting with < or > */
	case '>':
		c0.getnext();
		switch (c0.get_char()) {
		case '=':				/* >= */
			code = GE_OP; 
			val = ">=";
			break;
		case '>':
			c0.getnext();
			if (c0.get_char() == '=') {	/* >>= */
				code = RIGHT_ASSIGN;
				val = ">>=";
			} else {			/* >> */
				C::putback(c0);
				code = RIGHT_OP;
				val = ">>";
			}
			break;
		default:				/* > */
			C::putback(c0);
			val = (char)(code = '>');
			break;
		}
		break;
	case '<':
		if (context == cpp_include) {
			// C preprocessor #include <filename>
			val = "";
			for (;;) {
				c0.getnext();
				if (c0.get_char() == EOF || c0.get_char() == '>')
					break;
				val += c0.get_char();
			}
			code = PATHFNAME;
			break;
		}
		c0.getnext();
		switch (c0.get_char()) {
		case '=':				/* <= */
			code = LE_OP; 
			val = "<=";
			break;
		case '<':
			c0.getnext();
			if (c0.get_char() == '=') {	/* <<= */
				code = LEFT_ASSIGN;
				val = "<<=";
			} else {			/* << */
				code = LEFT_OP;
				val = "<<";
			}
			break;
		default:				/* < */
			C::putback(c0);
			val = (char)(code = '<');
			break;
		}
		break;
	/* Comments and / operators */
	case '/':
		c0.getnext();
		switch (c0.get_char()) {
		case '=':				/* /= */
			code = DIV_ASSIGN; 
			val = "/=";
			break;
		case '*':				/* Block comment */
			c0.getnext();
			for (;;) {
				while (c0.get_char() != '*' && c0.get_char() != EOF) {
					c0.getnext();
				}
				c0.getnext();
				if (c0.get_char() == EOF)
					Error::error(E_FATAL, "EOF in comment");
				if (c0.get_char() == '/')
					break;
			}
			code = SPACE;
			val = " ";
			break;
		case '/':				/* Line comment */
			do {
				c0.getnext();
			} while (c0.get_char() != '\n' && c0.get_char() != EOF);
			C::putback(c0);
			code = SPACE;
			val = " ";
			break;
		default:				/* / */
			C::putback(c0);
			val = (char)(code = '/');
			break;
		}
		break;
	case '.':	/* . and ... */
		c0.getnext();
		if (isdigit(c0.get_char())) {
			val = "." + c0.get_char();
			goto pp_number;
		}
		if (c0.get_char() != '.') {
			C::putback(c0);
			val = (char)(code = '.');
			break;
		}
		c1.getnext();
		if (c1.get_char() != '.') {
			C::putback(c1);
			C::putback(c0);
			val = (char)(code = '.');
			break;
		}
		code = ELLIPSIS;
		val = "...";
		break;
	/* 
	 * Convert whitespace into a single token; whitespace is needed
	 * by the C preprocessor.
	 */
	case ' ': case '\t': case '\v': case '\f': case '\r':
		do {
			c0.getnext();
		} while (c0.get_char() != EOF && isspace(c0.get_char()));
		C::putback(c0);
		val = " ";
		code = SPACE;
		break;
	/* Could be a long character or string */
	case 'L':
		c0.getnext();
		switch (c0.get_char()) {
		case '\'':
			goto char_literal;
		case '"':
			goto string_literal;
		default:
			C::putback(c0);
			goto identifier;
		}
	case '_': case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
	case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': 
	case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': 
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
	case 'H': case 'I': case 'J': case 'K': case 'M': case 'N': case 'O': 
	case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': 
	case 'W': case 'X': case 'Y': case 'Z': 
	identifier:
		{
		val = c0.get_char();
		Tokid base = c0.get_tokid();
		Tokid follow = base;
		for (;;) {
			c0.getnext();
			follow++;
			if (c0.get_char() == EOF ||
		            (!isalnum(c0.get_char()) && c0.get_char() != '_'))
		         	break;
			if (c0.get_tokid() != follow) {
				// Discontinuity; save the Tokids we have
				dequeTokid new_tokids = base.constituents(follow - base);
				copy(new_tokids.begin(), new_tokids.end(),
				     back_inserter(parts));
				follow = base = c0.get_tokid();
			}
			val += c0.get_char();
		}
		C::putback(c0);
		dequeTokid new_tokids = base.constituents(follow - base);
		copy(new_tokids.begin(), new_tokids.end(), back_inserter(parts));
		// Later it will become TYPE_NAME, IDENTIFIER, or reserved word
		code = IDENTIFIER;
		}
		break;
	case '\'':
	char_literal:
		n = 0;
		for (;;) {
			c0.getnext();
			if (c0.get_char() == '\\') {
				// Consume one character after the backslash
				// ... to deal with the '\'' problem
				val += '\\';
				c0.getnext();
				val += c0.get_char();
				// And let the loop decide with the next one
				c0.getnext();
				// We will deal with escapes later
				n++;
			}
			if (c0.get_char() == EOF || c0.get_char() == '\'')
				break;
			val += c0.get_char();
			n++;
		}
		val = CHAR_LITERAL;
		if (n == 0)
			Error::error(E_WARN, "Empty character literal");
		if (c0.get_char() == EOF)
			Error::error(E_ERR, "End of file in character literal");
		break;
	case '"':
	string_literal:
		if (context == cpp_include) {
			// C preprocessor #include "filename"
			for (;;) {
				c0.getnext();
				if (c0.get_char() == EOF || c0.get_char() == '"')
					break;
				val += c0.get_char();
			}
			code = ABSFNAME;
			break;
		}
		for (;;) {
			c0.getnext();
			if (c0.get_char() == '\\') {
				val += '\\';
				// Consume one character after the backslash
				c0.getnext();
				val += c0.get_char();
				// And let the loop decide with the next one
				c0.getnext();
				// We will deal with escapes later
			}
			if (c0.get_char() == EOF || c0.get_char() == '"')
				break;
			val += c0.get_char();
		}
		code = STRING_LITERAL;
		if (c0.get_char() == EOF)
			Error::error(E_ERR, "End of file in string literal");
		break;
	/* Various numbers */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': 
		val = c0.get_char();
	pp_number:
		for (;;) {
			c0.getnext();
			if (c0.get_char() == 'e' || c0.get_char() == 'E') {
				val += c0.get_char();
				c0.getnext();
				if (c0.get_char() == '+' || c0.get_char() == '-') {
					val += c0.get_char();
					continue;
				} else
					C::putback(c0);
			}
			if (c0.get_char() == EOF ||
		            (!isalnum(c0.get_char()) && c0.get_char() != '_'))
		         	break;
			val += c0.get_char();
		}
		C::putback(c0);
		code = PP_NUMBER;
		break;
	default:
		val = (char)(code = c0.get_char());
	}
}

#endif // PLTOKEN
