/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: ctoken.cpp,v 1.1 2001/09/02 09:23:58 dds Exp $
 */

#include <string>
#include <cctype>

#include "cpp.h"
#include "error.h"

/*
 * Return the character value of a string containing a C character
 * representation, advancing si to the next character to be processed.
 */
char
unescape_char(const string& s, string::const_iterator& si)
{
	if (*si != '\\')
		return *si++;
	if (++si == s.end()) {
		Error::error(E_ERR, "Unexpected end in character escape sequence");
		return 0;
	}
	switch (*si++) {
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
	default:
		Error::error(E_ERR, "Invalid character escape sequence");
		return 0;
	}
}
