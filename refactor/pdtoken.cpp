/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.2 2001/08/21 08:35:16 dds Exp $
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
	eat_to_eol();
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

	do {
		t.template getnext<Fchar>();
	} while (t.get_code() == SPACE);
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
	Fchar::set_input("test/toktest.c");

	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		cout << t;
	}

	return (0);
}
#endif /* UNIT_TEST */
