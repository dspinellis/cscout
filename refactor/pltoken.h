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
 * #include "ptoken.h"
 * #include "fchar.h"
 * #include "ytab.h"
 *
 * $Id: pltoken.h,v 1.4 2001/08/20 08:02:34 dds Exp $
 */

#ifndef PLTOKEN_
#define PLTOKEN_

class Fchar;

class Pltoken: public Ptoken {
public:
	Pltoken() {};
	template <class C> void getnext();
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
	C c0;

again:
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
#ifdef ndef
	case '-':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '-': t->set_props(DEC_OP, c0, cn1); delete(cn1); break;
		case '=': t->set_props(SUB_ASSIGN, c0, cn1); delete(cn1); break;
		case '>': t->set_props(PTR_OP, c0, cn1); delete(cn1); break;
		default:  in_unget(cn1); t->set_props('-', c0, c0); break;
		}
		delete (c0);
		break;
	case '&':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '&': t->set_props(AND_OP, c0, cn1); delete(cn1); break;
		case '=': t->set_props(AND_ASSIGN, c0, cn1); delete(cn1); break;
		default:  in_unget(cn1); t->set_props('&', c0, c0); break;
		}
		delete (c0);
		break;
	case '|':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '|': t->set_props(OR_OP, c0, cn1); delete(cn1); break;
		case '=': t->set_props(OR_ASSIGN, c0, cn1); delete(cn1); break;
		default:  in_unget(cn1); t->set_props('|', c0, c0); break;
		}
		delete (c0);
		break;
	/* Simple single/double character tokens (e.g. !, !=) */
	case '!':
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(NE_OP, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('!', c0, c0);
		}
		delete (c0);
		break;
	case '%':
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(MOD_ASSIGN, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('%', c0, c0);
		}
		delete (c0);
		break;
	case '*':
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(MUL_ASSIGN, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('*', c0, c0);
		}
		delete (c0);
		break;
	case '=':
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(EQ_OP, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('=', c0, c0);
		}
		delete (c0);
		break;
	case '^':
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(XOR_ASSIGN, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('^', c0, c0);
		}
		delete (c0);
		break;
	case '#':	/* C-preprocessor token only */
		incpp = true;			// Overkill, but good enough
		cn1 = in_get();
		if (cn1->get_char() == '=') {
			t->set_props(CPP_CONCAT, c0, cn1);
			delete (cn1);
		} else {
			in_unget(cn1);
			t->set_props('#', c0, c0);
		}
		delete (c0);
		break;
	/* Operators starting with < or > */
	case '>':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '=':				/* >= */
			t->set_props(GE_OP, c0, cn1); 
			delete (cn1);
			break;
		case '>':
			cn = in_get();
			if (cn->get_char() == '=') {	/* >>= */
				t->set_props(RIGHT_ASSIGN, c0, cn);
				delete (cn);
			} else {			/* >> */
				in_unget(cn);
				t->set_props(RIGHT_OP, c0, cn1);
			}
			delete (cn1);
			break;
		default:				/* > */
			in_unget(cn1);
			t->set_props('>', c0, c0);
			break;
		}
		delete (c0);
		break;
	case '<':
		if (incpp) {
			// C preprocessor #include <filename>
			cn = NULL;
			cn1 = c0;
			do {
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
			} while (cn1->get_char() != EOF && cn1->get_char() != '>');
			t->set_props(PATHFNAME, c0, cn1);
			delete(c0);
			if (cn != c0)
				delete(cn);
			if (cn1 != c0)
				delete(cn1);
			break;
		}
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '=':				/* <= */
			t->set_props(LE_OP, c0, cn1); 
			delete (cn1);
			break;
		case '<':
			cn = in_get();
			if (cn->get_char() == '=') {	/* <<= */
				t->set_props(LEFT_ASSIGN, c0, cn);
				delete (cn);
			} else {			/* << */
				in_unget(cn);
				t->set_props(LEFT_OP, c0, cn1);
			}
			delete (cn1);
			break;
		default:				/* < */
			in_unget(cn1);
			t->set_props('<', c0, c0);
			break;
		}
		delete (c0);
		break;
	/* Comments and / operators */
	case '/':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '=':				/* /= */
			t->set_props(DIV_ASSIGN, c0, cn1); 
			delete (c0);
			delete (cn1);
			break;
		case '*':				/* Block comment */
			delete (cn1);
			cn1 = in_get();
			for (;;) {
				while (cn1->get_char() != '*' && cn1->get_char() != EOF) {
					delete (cn1);
					cn1 = in_get();
				}
				delete (cn1);
				cn1 = in_get();
				if (cn1->get_char() == EOF)
					error(E_FATAL, c0, "EOF in comment");
				if (cn1->get_char() == '/')
					break;
			}
			t->set_props(SPACE, c0, cn1);
			delete (c0);
			delete (cn1);
			break;
		case '/':				/* Line comment */
			delete (cn1);
			cn = NULL;
			cn1 = c0;
			do {
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
			} while (cn1->get_char() != '\n' && cn1->get_char() != EOF);
			in_unget(cn1);
			t->set_props(SPACE, c0, cn);
			delete (c0);
			if (cn != c0)
				delete (cn);
			break;
		default:				/* / */
			in_unget(cn1);
			t->set_props('/', c0, c0);
			delete (c0);
			break;
		}
		break;
	case '.':	/* . and ... */
		cn1 = in_get();
		if (isdigit(cn1->get_char())) {
			cn = NULL;
			goto float_fraction;
		}
		if (cn1->get_char() != '.') {
			in_unget(cn1);
			t->set_props('.', c0, c0);
			delete (c0);
			break;
		}
		cn = in_get();
		if (cn->get_char() != '.') {
			in_unget(cn);
			in_unget(cn1);
			t->set_props('.', c0, c0);
			delete (c0);
			break;
		}
		t->set_props(ELLIPSIS, c0, cn);
		delete (c0);
		delete (cn1);
		delete (cn);
		break;
#endif // ndef
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
#ifdef ndef
	/* Could be a long character or string */
	case 'L':
		cn1 = in_get();
		switch (cn1->get_char()) {
		case '\'':
			delete(cn1);
			goto char_literal;
		case '"':
			delete(cn1);
			goto string_literal;
		default:
			in_unget(cn1);
			goto identifier;
		}
#endif //ndef
	case '_': case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': 
	case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': 
	case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': 
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
	case 'H': case 'I': case 'J': case 'K': case 'M': case 'N': case 'O': 
	case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': 
	case 'W': case 'X': case 'Y': case 'Z': 
	identifier:
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
		break;
#ifdef ndef
	case '\'':
	char_literal:
		cn = NULL;
		cn1 = c0;
		n = 0;
		do {
			if (cn && cn != c0)
				delete (cn);
			cn = cn1;
			cn1 = in_get();
			if (cn1->get_char() == '\\') {
				// Consume one character after the backslash
				// ... to deal with the '\'' problem
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
				// And let the loop decide with the next one
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
				// We will deal with escapes later
				n++;
			}
			n++;
		} while (cn1->get_char() != EOF && cn1->get_char() != '\'');
		t->set_props(INT_CONST, c0, cn1);
		if (n == 1)
			error(E_WARN, c0, "Empty character literal");
		if (cn1->get_char() == EOF)
			error(E_ERR, c0, "End of file in character literal");
		delete(c0);
		if (cn != c0)
			delete(cn);
		if (cn1 != c0)
			delete(cn1);
		break;
	case '"':
	string_literal:
		if (incpp) {
			// C preprocessor #include "filename"
			cn = NULL;
			cn1 = c0;
			do {
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
			} while (cn1->get_char() != EOF && cn1->get_char() != '"');
			t->set_props(ABSFNAME, c0, cn1);
			delete(c0);
			if (cn != c0)
				delete(cn);
			if (cn1 != c0)
				delete(cn1);
			break;
		}
		cn = NULL;
		cn1 = c0;
		do {
			if (cn && cn != c0)
				delete (cn);
			cn = cn1;
			cn1 = in_get();
			if (cn1->get_char() == '\\') {
				// Consume one character after the backslash
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
				// And let the loop decide with the next one
				if (cn && cn != c0)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
				// We will deal with escapes later
			}
		} while (cn1->get_char() != EOF && cn1->get_char() != '"');
		t->set_props(STRING_LITERAL, c0, cn1);
		if (cn1->get_char() == EOF)
			error(E_ERR, c0, "End of file in string literal");
		delete(c0);
		if (cn != c0)
			delete(cn);
		if (cn1 != c0)
			delete(cn1);
		break;
	/* Various numbers */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': 
		cn1 = in_get();
		if (cn1->get_char() == 'x' || cn1->get_char() == 'X') {
			// Hex constant
			cn = NULL;
			n = 0;
			do {
				if (cn)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
				n++;
			} while (isxdigit(cn1->get_char()));
			if (n == 1)
				error(E_ERR, c0, "Missing digit in hex constant");
		} else {
			// Decimal constant
			cn = NULL;
			while (isdigit(cn1->get_char())) {
				if (cn)
					delete (cn);
				cn = cn1;
				cn1 = in_get();
			}
			switch (cn1->get_char()) {
			case 'e': case 'E':
				goto float_exp;
			case '.':
				if (cn)
					delete(cn);
				cn = cn1;
				cn1 = in_get();
				goto float_fraction;
			}
		}
		// Continue with integer suffix
		bool have_u, have_l;
		have_u = have_l = false;
		for (;;) {
			switch (cn1->get_char()) {
			case 'u': case 'U':
				if (have_u)
					error(E_ERR, c0, "Bad integer suffix");
				have_u = true;
				break;
			case 'l': case 'L':
				if (have_l)
					error(E_ERR, c0, "Bad integer suffix");
				have_l = true;
				break;
			default:
				goto suffix_end;
			}
			if (cn)
				delete(cn);
			cn = cn1;
			cn1 = in_get();
		}
	suffix_end:
		in_unget(cn1);
		if (cn)
			t->set_props(INT_CONST, c0, cn);
		else
			t->set_props(INT_CONST, c0, c0);
		delete(c0);
		delete(cn);
		break;
	float_fraction:
		/*
		 * cn1 contains the first character after the '.'
		 * It is either a digit, or we have already seen
		 * digits before the ., so we know the number has at least
		 * one digit.
		 */
		 while (isdigit(cn1->get_char())) {
			if (cn)
				delete (cn);
			cn = cn1;
			cn1 = in_get();
		}
		if (cn1->get_char() == 'e' || cn1->get_char() == 'E') {
	float_exp:
			if (cn)
				delete(cn);
			cn = cn1;
			cn1 = in_get();
			if (cn1->get_char() == '+' || cn1->get_char() == '-') {
				delete(cn);
				cn = cn1;
				cn1 = in_get();
			}
			n = 0;
			while (isdigit(cn1->get_char())) {
				delete (cn);
				cn = cn1;
				cn1 = in_get();
				n++;
			}
			if (n == 0)
				error(E_ERR, c0, "Malformed floating point exponent");
		}
		// Floating point suffix (in cn1)
		switch (cn1->get_char()) {
		case 'f': case 'F': case 'l': case 'L':
			if (cn)
				delete(cn);
			cn = cn1;
			break;
		default:
			in_unget(cn1);
		}
		t->set_props(FLOAT_CONST, c0, cn);
		delete(c0);
		delete(cn);
		break;
	default:
		error(E_WARN, c0, "Bad character value %c (%d - 0x%02x)", 
			c0->get_char(), c0->get_char(), c0->get_char());
		goto again;
#endif  //ndef
	}
}
#endif // PLTOKEN
