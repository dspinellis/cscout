/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: ctoken.cpp,v 1.14 2002/09/13 12:42:06 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <cassert>

#include "cpp.h"
#include "debug.h"
#include "fileid.h"
#include "attr.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "parse_tab.h"

/*
 * Return the character value of a string containing a C character
 * representation, advancing si to the next character to be processed.
 */
char
unescape_char(const string& s, string::const_iterator& si)
{
	int val;

	if (*si != '\\')
		return *si++;
	if (++si == s.end()) {
		Error::error(E_ERR, "Unexpected end in character escape sequence");
		return 0;
	}
	switch (val = *si++) {
	case '\'': return '\'';
	case '"': return '"';
	case '?': return '?';
	case '\\': return '\\';
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';

	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		val -= '0';
		// Up to three digits maximum; strtol is useless.
		for (int i = 0; i < 2; i++) {
			if (si == s.end() || *si < '0' || *si > '7')
				break;
			val *= 8;
			val += *si++ - '0';
		}
		// if (si != s.end()) cout << "Next char is [" << *si << "]\n";
		return (val);
	case 'x': {
		string tmp(si - 2, s.end());
		char *endptr;
		tmp[0] = '0';		// For strtoul 0xddd
		// cout << "tmp=" << tmp << "\n";
		const char *cs = tmp.c_str();
		val = strtol(cs, &endptr, 0);
		if (endptr - cs == 1)
			// All we could read was 0x
			Error::error(E_ERR, "Illegal characters in hex escape sequence");
		si += endptr - cs - 2;
		// if (si != s.end()) cout << "Next char is [" << *si << "]\n";
		return (val);
	}

	default:
		Error::error(E_ERR, "Invalid character escape sequence");
		return 0;
	}
}

// Create the map from identifiers to keyword token values
static map<string,int> &
make_keymap()
{
	static map<string,int> m;

	m["auto"] = AUTO; m["double"] = DOUBLE; m["int"] = INT;
	m["struct"] = STRUCT; m["break"] = BREAK; m["else"] = ELSE;
	m["long"] = LONG; m["switch"] = SWITCH; m["case"] = CASE;
	m["enum"] = ENUM; m["register"] = REGISTER; m["typedef"] = TYPEDEF;
	m["char"] = CHAR; m["extern"] = EXTERN; m["return"] = RETURN;
	m["union"] = UNION; m["const"] = TCONST; m["float"] = FLOAT;
	m["short"] = SHORT; m["unsigned"] = UNSIGNED; m["continue"] = CONTINUE;
	m["for"] = FOR; m["signed"] = SIGNED; m["void"] = TVOID;
	m["default"] = DEFAULT; m["goto"] = GOTO; m["sizeof"] = SIZEOF;
	m["volatile"] = VOLATILE; m["do"] = DO; m["if"] = IF;
	m["static"] = STATIC; m["while"] = WHILE;
	m["__asm__"] = GNUC_ASM; m["_asm"] = MSC_ASM;
	m["__typeof"] = TYPEOF;
	return m;
}

// Map from identifiers to keyword token values
static map<string,int>& keymap = make_keymap();

static int parse_lex_real();

// Consume tokens within an asm block
// The block can be prefixed by identifiers like volatile (gcc)
static void
eat_block(int open, int close)
{
	int matches = 1;
	int tok;

	do {
		tok = parse_lex_real();
	} while (tok != IDENTIFIER && tok != open);
	if (tok != open)
		Error::error(E_ERR, "asm block syntax");
	do {
		tok = parse_lex_real();
		if (tok == open)
			matches++;
		else if (tok == close)
			matches--;
	} while (matches > 0);
}

// Lexical analysis function for yacc
static int
parse_lex_real()
{
	int c;
	Id const *id;
	map<string,int>::const_iterator ik;
	extern YYSTYPE parse_lval;

	for (;;) {
		Pdtoken t;
		t.getnext();
		switch (c = t.get_code()) {
		case '\n':
		case SPACE:
			continue;
		case EOF:
			return (0);
		case ABSFNAME:
		case PATHFNAME:
			Error::error(E_INTERNAL, "preprocessor filename, past perprocessor");
			continue;
		case PP_NUMBER:
		case CHAR_LITERAL:
			// XXX Could also be invalid, or FLOAT_CONST
			return (INT_CONST);
		case IDENTIFIER:
			if (DP()) cout << "id: [" << t.get_val() << "]\n";
			ik = keymap.find(t.get_val());
			if (ik != keymap.end())
				// Keyword
				switch ((*ik).second) {
				case GNUC_ASM:
					eat_block('(', ')');
					continue;
				case MSC_ASM:
					eat_block('{', '}');
					continue;
				default:
					return (*ik).second;
				}
			id = obj_lookup(t.get_val());
			parse_lval.t = identifier(t);
			if (id && id->get_type().is_typedef())
				return (TYPEDEF_NAME);	// Probably typedef
			return (IDENTIFIER);		// Plain identifier
		default:
			return (c);
		}
	}
}

// Lexical analysis function for yacc
// Used for debugging
int
parse_lex()
{
	int l = parse_lex_real();
	if (DP())
		cout << "Parse lex returns " << Token(l) << "\n";
	return (l);
}
