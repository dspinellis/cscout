/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A preprocessor lexical token.
 * The getnext() method for these tokens converts characters into tokens.
 *
 * $Id: pltoken.h,v 1.38 2010/10/27 20:19:35 dds Exp $
 */

#ifndef PLTOKEN_
#define PLTOKEN_

#include "debug.h"
#include "tokid.h"
#include "ptoken.h"
#include "call.h"

class Fchar;

// C-preprocessor context influences lexical analysis
enum e_cpp_context {
	cpp_normal,	// Normal operation
	cpp_include,	// Set while processing a #include directive (will recognize ABSFNAME and PATHFNAME)
	cpp_define	// Set while processing a #define directive (will recognize CONCAT)
};

// A C preprocessor lexical token
class Pltoken: public Ptoken {
private:
	static enum e_cpp_context context;
	// Allow line comments starting with a semicolon (inside Microsoft asm)
	static bool semicolon_line_comments;
	// Echo characters read on standard output
	static bool echo;
	template <class C> void update_parts(Tokid& base, Tokid& follow, const C& c0);
	Tokid t;		// Token identifier for delimeters: comma, bracket
	template <class C> void getnext_analyze();
public:
	template <class C> void getnext();
	template <class C> void getnext_nospc();
	static void set_context(enum e_cpp_context con) { context = con; };
	static void set_semicolon_line_comments(bool v) { semicolon_line_comments = v; }
	Tokid get_delimiter_tokid() const { return t; }
	static void set_echo() { echo = true; }
	static void clear_echo() { echo = false; }
};

/*
 * Given "base" that marks the beginning of a token
 * "follow" that follows its characters as they are read, and
 * c0, a new character read, check that the new character
 * is indeed agreeing with the value of "follow".
 * If not update "parts" and reset "follow" and "base".
 */
template <class C>
void
Pltoken::update_parts(Tokid& base, Tokid& follow, const C& c0)
{
	if (c0.get_tokid() != follow) {
		// Discontinuity; save the Tokids we have
		dequeTpart new_tokids = base.constituents(follow - base);
		copy(new_tokids.begin(), new_tokids.end(),
		     back_inserter(parts));
		follow = base = c0.get_tokid();
	}
}

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
Pltoken::getnext_analyze()
{
	int n;
	C c0, c1;
	Tokid base, follow;
	dequeTpart new_tokids;

	parts.clear();
	c0.getnext();
	switch (c0.get_char()) {
	/*
	 * Single character C operators and punctuators
	 * ANSI 3.1.5 p. 32 and 3.1.6 p. 33
	 */
	case '\n':	// Needed for processing directives
		context = cpp_normal;
		// FALLTRHOUGH
	case '[': case ']': case '(': case ')':
	case '~': case '?': case ':': case ',':
	case '{': case '}':
	case EOF:
		val = (char)(code = c0.get_char());
		t = c0.get_tokid();
		break;
	case ';':
		if (semicolon_line_comments)
			goto line_comment;
		else
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
			break;
		}
		// Yacc tokens
		if (Fchar::is_yacc_file()) {
			extern bool parse_yacc_defs;

			if (c0.get_char() == '%') {
				val = "%%";
				code = YMARK;
				break;
			}
			if (c0.get_char() == '{') {
				val = "%{";
				code = YLCURL;
				parse_yacc_defs = false;
				break;
			}
			if (c0.get_char() == '}') {
				val = "%}";
				code = YRCURL;
				parse_yacc_defs = true;
				break;
			}
		}
		C::putback(c0);
		val = (char)(code = '%');
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
		if (context == cpp_define && c0.get_char() == '#') {
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
				C::putback(c0);
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
			// Do not delete comments from expanded macros
			if (!C::is_file_source())
				goto no_comment;
			c0.getnext();
			for (;;) {
				while (c0.get_char() != '*' && c0.get_char() != EOF) {
					c0.getnext();
				}
				c0.getnext();
				if (c0.get_char() == EOF)
					/*
					 * @error
					 * The end of file was reached while
					 * processing a block comment
					 */
					Error::error(E_FATAL, "EOF in comment");
				if (c0.get_char() == '/')
					break;
			}
			code = SPACE;
			val = " ";
			break;
		case '/':				/* Line comment */
			// Do not delete comments from expanded macros
			if (!C::is_file_source())
				goto no_comment;
		line_comment:
			do {
				c0.getnext();
			} while (c0.get_char() != '\n' && c0.get_char() != EOF);
			C::putback(c0);
			code = SPACE;
			val = " ";
			break;
		no_comment:
			/*
			 * Comment in an expanded macro.
			 * Could issue a warning here, but Microsoft uses such
			 * line comments, so we handle it in pdtoken.cpp
			 */
		default:				/* / */
			C::putback(c0);
			val = (char)(code = '/');
			break;
		}
		break;
	case '.':	/* . and ... */
		follow = base = c0.get_tokid();
		c0.getnext();
		follow++;
		if (isdigit(c0.get_char())) {
			update_parts(base, follow, c0);
			val = string(".") + (char)(c0.get_char());
			if (DP())
				cout << "val=[" << val << "]\n";
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
		} while (c0.get_char() != EOF && c0.get_char() != '\n' && isspace(c0.get_char()));
		C::putback(c0);
		val = " ";
		code = SPACE;
		break;
	/* Could be a long character or string */
	case 'L':
		c1.getnext();
		switch (c1.get_char()) {
		case '\'':
			goto char_literal;
		case '"':
			goto string_literal;
		default:
			C::putback(c1);
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
		if (DP()) cout << "Base:" << base << "\n";
		Tokid follow = base;
		for (;;) {
			c0.getnext();
			follow++;
			if (c0.get_char() == EOF ||
		            (!isalnum(c0.get_char()) && c0.get_char() != '_'))
		         	break;
			update_parts(base, follow, c0);
			val += c0.get_char();
		}
		C::putback(c0);
		dequeTpart new_tokids = base.constituents(follow - base);
		copy(new_tokids.begin(), new_tokids.end(), back_inserter(parts));
		// Later it will become TYPE_NAME, IDENTIFIER, or reserved word
		code = IDENTIFIER;
		}
		break;
	case '\'':
	char_literal:
		n = 0;
		val = "";
		for (;;) {
			c0.getnext();
			if (c0.get_char() == '\\') {
				// Consume one character after the backslash
				// ... to deal with the '\'' problem
				val += '\\';
				c0.getnext();
				if (c0.get_char() == EOF) {
					/*
					 * @error
					 * The end of file was reached while
					 * processing a character literal:
					 * a single quote was never closed
					 */
					Error::error(E_ERR, "End of file in character literal");
					break;
				}
				val += c0.get_char();
				// We will deal with escapes later
				n++;
				continue;
			}
			if (c0.get_char() == EOF || c0.get_char() == '\'')
				break;
			val += c0.get_char();
			n++;
		}
		code = CHAR_LITERAL;
		if (n == 0)
			/*
			 * @error
			 * Character lirerals must include a character
			 */
			Error::error(E_WARN, "Empty character literal");
		if (c0.get_char() == EOF)
			Error::error(E_ERR, "End of file in character literal");
		break;
	case '"':
	string_literal:
		val = "";
		if (context == cpp_include) {
			// C preprocessor #include "filename"
			for (;;) {
				c0.getnext();
				if (c0.get_char() == EOF || c0.get_char() == '\n' || c0.get_char() == '"')
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
				if (c0.get_char() == EOF || c0.get_char() == '\n')
					break;
				val += c0.get_char();
				// We will deal with escapes later
				continue;
			}
			if (c0.get_char() == EOF || c0.get_char() == '\n' || c0.get_char() == '"')
				break;
			val += c0.get_char();
		}
		code = STRING_LITERAL;
		if (c0.get_char() == EOF)
			/*
			 * @error
			 * The end of the file was reached while
			 * processing a string
			 */
			Error::error(E_ERR, "End of file in string literal");
		if (c0.get_char() == '\n')
			/*
			 * @error
			 * The end of the line was reached while
			 * processing a string
			 */
			Error::error(E_ERR, "End of line in string literal");
		break;
	/* Various numbers */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		val = c0.get_char();
		follow = base = c0.get_tokid();
	pp_number:
		for (;;) {
			c0.getnext();
			follow++;
			if (c0.get_char() == 'e' || c0.get_char() == 'E') {
				update_parts(base, follow, c0);
				val += c0.get_char();
				c0.getnext();
				follow++;
				if (c0.get_char() == '+' || c0.get_char() == '-') {
					update_parts(base, follow, c0);
					val += c0.get_char();
					continue;
				}
			}
			if (c0.get_char() == EOF ||
		            (!isalnum(c0.get_char()) && c0.get_char() != '.' && c0.get_char() != '_'))
		         	break;
			update_parts(base, follow, c0);
			val += c0.get_char();
		}
		C::putback(c0);
		new_tokids = base.constituents(follow - base);
		copy(new_tokids.begin(), new_tokids.end(), back_inserter(parts));
		code = PP_NUMBER;
		break;
	default:
		val = (char)(code = c0.get_char());
	}
	Call::process_token(*this);
	// For metric counting filter out whitespace
	if (code != SPACE && code != '\n')
		Metrics::call_metrics(&Metrics::add_pptoken);
	if (DP()) cout << "getnext returns: " << *this << "\n";
}

template <class C>
void
Pltoken::getnext()
{
	getnext_analyze<C>();
	if (echo)
		cout << get_c_val();
}

template <class C>
void
Pltoken::getnext_nospc()
{
	do {
		getnext<C>();
	} while (code == SPACE);
}

#endif // PLTOKEN
