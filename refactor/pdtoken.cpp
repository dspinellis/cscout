/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.7 2001/08/23 20:53:27 dds Exp $
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
	Pltoken::set_context(cpp_include);
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

	Pltoken::set_context(cpp_define);
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

typedef list<Pltoken> listPltoken;
typedef set<string> setstring;
typedef map<string, listPltoken> mapArgval;

static bool
gather_args(listPltoken::iterator pos, const dequePltoken& formal_args, mapArgval& args, bool get_more)
{
}

/*
 * Check for macro at token position pos and possibly expand it  
 * If a macro is expanded, pos is invalidated and replaced with the replacement 
 * macro value.
 * Macros that are members of the tabu set are not expanded.
 * If get_more is true, more data can be retrieved from Pltoken::get_next
 * Return true if a  replacemement was made
 */
bool
macro_replace(listPdtoken& tokens, listPltoken::iterator pos, setstring& tabu, bool get_more)
{
	mapMacro::const_iterator mi;
	Macro& m;
	iterator expand_end;
	string& name = (*pos).get_val();
	if ((mi = macros.find(name)) == macros.end() || tabu.find(name) != tabu.end())
		return (false);
	listPlotken::iterator expand_start = pos + 1;	// For rescan
	tokens.erase(pos);
	pos = expand_start();
	tabu.insert(name);
	m = *mi;
	if (m.is_function) {
		int nargs = m.formal_args.count();
		mapArgval args;			// Map from formal name to value
		gather_args(pos, m.formal_args, args, get_more);
		dequePltoken::const_iterator i;
		// Substitute with macro's replacement value
		for(i = m.value.begin(); i != m.value.end(); i++) {
			// Is it a stringizing operator ?
			if ((*i).get_code() == '#') {
				if (i + 1 == m.value.end()) {
					Error:error(E_ERR,  "Application of macro \"" + name + "\": operator # at end of macro pattern");
					return (false);
				}
				do_stringize = true;
				i++;
			}
			mapArgval::const_iterator ai;
			// Is it a formal argument?
			if ((ai = args.find((*i).get_val())) != args.end()) {
				// Yes, see if macro-replacement allowed
				// ANSI 3.8.3.1 p. 91
				// Not preceded by # or ## or followed by ##
				if ((i == m.value.begin() ||
				     ((*(i - 1)).get_code() != '#' &&
				      (*(i - 1)).get_code() != CPP_CONCAT)) &&
				    (i + 1 == m.value.end() ||
				     (*(i + 1)).get_code != CPP_CONCAT))
					// Allowed, macro replace the parameter
					macro_replace((*ai).second, (*ai).second.begin(), tabu, false);
				if (do_stringize)
					tokens.insert(pos, stringize((*ai).second));
				else
					copy((*ai).second.begin(), (*ai).second.end(), inserter(tokens, pos));
			} else {
				// Not a formal argument, plain replacement
				// Check for misplaced # operator (3.8.3.2)
				if (do_stringize)
					Error:error(E_WARN, "Application of macro \"" + name + "\": operator # only valid before macro parameters");
				tokens.insert(pos, *i);
			}
		}
	} else {
		// Object-like macro
		tokens.insert(pos, m.value.begin(), m.value.end());
	}

	// Check and apply CPP_CONCAT (ANSI 3.8.3.3)
	listPlotken::iterator ti, next;
	for (ti = tokens.begin(); ti != tokens.end(); ti = next)
		if (ti + 1 != tokens.end() && ti + 2 != tokens.end() &&
		    (*(ti + 1)).get_code() == CPP_CONCAT) {
			next = ti + 3;
			Schar::push_input(*ti);
			Schar::push_input(*(ti + 2));
			tokens.erase(ti, next);
			for (;;) {
				Pltoken t;
				t.template getnext<Schar>();
				if (t.get_code() == EOF)
					break;
				tokens.insert(next, t);
			}
		} else
			next = ti + 1;
	
	// Continously rescan the sequence while replacements are made
	for (;;) {
		bool replaced = false;
		// We should start at expand_start, but this may be
		// invalided by a replacement
		for (ti = tokens.begin(); ti != tokens.end(); ti++)
			if (macro_replace(tokens, ti, tabu, true)) {
				replaced = true;
				break;
			}
		if (!replaced)
			return (true);
	}
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
