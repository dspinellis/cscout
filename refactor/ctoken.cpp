/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: ctoken.cpp,v 1.3 2001/09/08 17:31:14 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <cassert>

#include "cpp.h"
#include "debug.h"
#include "fileid.h"
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
#include "parse.tab.h"

bool Cidtoken::typedef_context = true;

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

int
parse_lex_real()
{
	int c;

	for (;;) {
		Keyword k;
		Pdtoken t = t.getnext();
		switch (c = t.get_code()) {
		case SPACE:
			continue;
		case EOF:
			return (0);
		case ABSFNAME:
		case PATHFNAME:
			Error:error(E_INTERNAL, "preprocessor filename, past perprocessor");
			continue;
		case PP_NUMBER:
		case CHAR_LITERAL:
			// XXX Could also be invalid, or FLOAT_CONST
			return (INT_CONST);
		case IDENTIFIER:
			k = Ctoken::lookup_keyword(t.get_val());
			if (k.id) {
				// It is a keyword
				if (k.is_type_spec) 
				switch (c) {
				case 
				re
			if (Cidtoken::typedef_context &&
		default:
			return (c);
		}
	}
}

// Used for debugging
int
parse_lex()
{
	int l = parse_lex_real();
	if (DP())
		cout << "Parse lex returns " << Token(l) << "\n";
	return (l);
}
