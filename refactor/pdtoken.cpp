/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.4 2001/08/21 20:06:07 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "ytab.h"
#include "error.h"
#include "pltoken.h"
#include "pdtoken.h"

bool Pdtoken::at_bol = true;
listPdtoken Pdtoken::expand;
mapMacro Pdtoken::macros;			// Defined macros

void
Pdtoken::getnext()
{
	Pltoken t;

expand_get:
	if (!expand.empty()) {
		*this = expand.front();
		expand.pop_front();
		return;
	}
again:
	t.template getnext<Fchar>();
	if (at_bol) {
		switch (t.get_code()) {
		case SPACE:
		case '\n':
			goto again;
		case '#':
			process_directive();
			goto again;
		default:
			at_bol = false;
		}
	}
	switch (t.get_code()) {
	case '\n':
		at_bol = true;
		//goto again; To remove newlines at this step
		*this = t;
		break;
	case IDENTIFIER:
		if (macros.find(t.get_val()) != macros.end()) {
			expand_macro(t);
			goto expand_get;
		}
		// FALLTRHOUGH
	default:
		*this = t;
		break;
	}
}

void
Pdtoken::expand_macro(Pltoken t)
{
}

// Consume input up to (and including) the first \n
void
Pdtoken::eat_to_eol()
{
	Pltoken t;

	do {
		t.template getnext<Fchar>();
	} while (t.get_code() != EOF && t.get_code() != '\n');
}

void
Pdtoken::process_if(enum e_if_type)
{
	eat_to_eol();
}

void
Pdtoken::process_else()
{
	eat_to_eol();
}

void
Pdtoken::process_endif()
{
	eat_to_eol();
}

void
Pdtoken::process_include()
{
	eat_to_eol();
}

void
Pdtoken::process_define()
{
	Macro m;
	string name;
	typedef map <string, Token> mapToken;	// To unify args with body
	mapToken args;
	Pltoken t;

	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		Error::error(E_ERR, "Invalid macro name");
		eat_to_eol();
		return;
	}
	name = t.get_val();
	t.template getnext<Fchar>();	// Space is significant: a(x) vs a (x)
	m.is_function = false;
	if (t.get_code() == '(') {
		// Function-like macro
		m.is_function = true;
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != ')')
			// Formal args follow; gather them
			for (;;) {
				if (t.get_code() != IDENTIFIER) {
					Error::error(E_ERR, "Invalid macro parameter name");
					eat_to_eol();
					return;
				}
				args[t.get_val()] = t;
				m.formal_args.push_back(t);
				t.template getnext_nospc<Fchar>();
				if (t.get_code() == ')')
					break;
				if (t.get_code() != ',') {
					Error::error(E_ERR, "Invalid macro parameter punctuation");
					eat_to_eol();
					return;
				}
				t.template getnext_nospc<Fchar>();
			}
	}
	// Continue gathering macro body
	// Space is significant for comparing same definitions!
	for (;;) {
		t.template getnext<Fchar>();
		if (t.get_code() == '\n')
			break;
		m.value.push_back(t);
		mapToken::const_iterator i;
		if ((i = args.find(t.get_val())) != args.end())
			unify(t, (*i).second);
	}
	// Check that the new macro is not different from an older definition
	mapMacro::const_iterator i = macros.find(name);
	if (i != macros.end() && (*i).second != m)
		Error::error(E_WARN, "Duplicate (different) macro definition");
	macros[name] = m;
}

void
Pdtoken::process_undef()
{
	eat_to_eol();
}

void
Pdtoken::process_line()
{
	eat_to_eol();
}

void
Pdtoken::process_error()
{
	eat_to_eol();
}

void
Pdtoken::process_pragma()
{
	eat_to_eol();
}

void
Pdtoken::process_directive()
{
	Pltoken t;
	bool if_val;

	t.template getnext_nospc<Fchar>();
	if (t.get_code() == '\n')		// Empty directive
		return;
	if (t.get_code() != IDENTIFIER) {
		Error::error(E_ERR, "Preprocessor syntax");
		eat_to_eol();
		return;
	}
	if (t.get_val() == "define")
		process_define();
	else if (t.get_val() == "include")
		process_include();
	else if (t.get_val() == "if")
		process_if(if_plain);
	else if (t.get_val() == "ifdef")
		process_if(if_def);
	else if (t.get_val() == "ifndef")
		process_if(if_ndef);
	else if (t.get_val() == "elif")
		process_if(if_elif);
	else if (t.get_val() == "else")
		process_else();
	else if (t.get_val() == "endif")
		process_endif();
	else if (t.get_val() == "undef")
		process_undef();
	else if (t.get_val() == "line")
		process_line();
	else if (t.get_val() == "error")
		process_error();
	else if (t.get_val() == "pragma")
		process_pragma();
	else
		Error::error(E_ERR, "Unknown preprocessor directive: " + t.get_val());
}


#ifdef UNIT_TEST

main()
{
	Fchar::set_input("test/pdtest.c");

	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		cout << t;
	}
	cout << "Tokid map:\n";
	cout << tokid_map;

	return (0);
}
#endif /* UNIT_TEST */
