/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.21 2001/08/31 19:03:51 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <algorithm>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "pdtoken.h"
#include "tchar.h"

bool Pdtoken::at_bol = true;
listPtoken Pdtoken::expand;
mapMacro Pdtoken::macros;			// Defined macros
stackbool Pdtoken::iftaken;		// Taken #ifs
bool Pdtoken::skipping;			// Skip tokens when true
int Pdtoken::iflevel;			// Level of enclosing #ifs

void
Pdtoken::getnext()
{
	Pltoken t;
	setstring tabu;				// For macro replacement

expand_get:
	if (!expand.empty()) {
		Pdtoken n(expand.front());
		*this = n;
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
		expand.push_front(t);
		tabu.clear();
		macro_replace(expand, expand.begin(), tabu, true);
		goto expand_get;
		// FALLTRHOUGH
	default:
		*this = t;
		break;
	}
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

/*
 * Read tokens comprising a cpp expression up to the newsline and return
 * its value.
 * Algorithm:
 * -. Read tokens.
 * -. Process defined operator
 * -. Macro-expand sequence
 * -. Replace all identifiers with 0
 * -. Parse and evaluate sequence
 */
static int
eval()
{
	Pltoken t;
	t.template getnext_nospc<Fchar>();
}

/*
 * Algorithm for processing #if* #else #elif #endif sequences:
 * Each #if is evaluated and the result is pushed on the iftaken stack.
 * If false skip to next matching control.
 * At #elif check iftaken.top(): if true, skip to next matching control,
 * if false, pop stack and work like an if.
 * At #else check iftaken.top(): if true, skip to next matching control,
 * if false continue processing input.
 * At #endif execute iftaken.pop()
 *
 * Skipping is performed by setting level=0 and looking for a 
 * #e* when level == 0.
 * While skipping each #if* results in level++, each #endif in level--.
 */
void
Pdtoken::process_if(enum e_if_type type)
{
	if (skipping)
		iflevel++;
	else {
		if (type == if_elif) {
			if (iftaken.top()) {
				skipping = true;
				iflevel = 0;
				return;
			} else
				iftaken.pop();
		}
		bool eval_res = true;		// XXX
		iftaken.push(eval_res);
		if (!eval_res) {
			skipping = true;
			iflevel = 0;
		}
	}
	eat_to_eol();
}

void
Pdtoken::process_else()
{
	if (skipping && iflevel > 0)
		return;
	if (iftaken.top()) {
		skipping = true;
		iflevel = 0;
		return;
	} else
		skipping = false;
	eat_to_eol();
}

void
Pdtoken::process_endif()
{
	iflevel--;
	iftaken.pop();
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
	m.name_token = t;
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


/*
 * Return a macro argument token from tokens position pos.
 * Used by gather_args.
 * If get_more is true when tokens is exhausted read using pltoken::getnext
 * Update pos to the first token not gathered.
 * If want_space is true return spaces, otherwise discard them
 */
Ptoken
arg_token(listPtoken& tokens, listPtoken::iterator& pos, bool get_more, bool want_space)
{
	if (want_space) {
		if (pos != tokens.end())
			return (*pos++);
		if (get_more) {
			Pltoken t;
			t.template getnext<Fchar>();
			return (t);
		}
		return Ptoken(EOF, "");
	} else {
		while (pos != tokens.end() && (*pos).is_space())
			pos++;
		if (pos != tokens.end())
			return (*pos++);
		if (get_more) {
			Pltoken t;
			do {
				t.template getnext_nospc<Fchar>();
			} while (t.get_code() != EOF && t.is_space());
			return (t);
		}
		return Ptoken(EOF, "");
	}
}
				
/*
 * Get the macro arguments specified in formal_args, initiallly from pos,
 * then, if get_more is true, from pltoken<fchar>.getnext.
 * Build the map from formal name to argument value args.
 * Update pos to the first token not gathered.
 * Return true if ok, false on error.
 */
static bool
gather_args(const string& name, listPtoken& tokens, listPtoken::iterator& pos, const dequePtoken& formal_args, mapArgval& args, bool get_more)
{
	Ptoken t;
	t = arg_token(tokens, pos, get_more, false);
	if (t.get_code() != '(') {
		Error::error(E_ERR, "macro [" + name + "]: open bracket expected for function-like macro");
		return false;
	}
	dequePtoken::const_iterator i;
	for (i = formal_args.begin(); i != formal_args.end(); i++) {
		listPtoken& v = args[(*i).get_val()];
		char term = (i + 1 == formal_args.end()) ? ')' : ',';
		int bracket = 0;
		// Get a single argument
		for (;;) {
			t = arg_token(tokens, pos, get_more, true);
			if (bracket == 0 && t.get_code() == term)
				break;
			switch (t.get_code()) {
			case '(':
				bracket++;
				break;
			case ')':
				bracket--;
				break;
			case EOF:
				Error::error(E_ERR, "macro [" + name + "]: EOF while reading function macro arguments");
				return (false);
			}
			v.push_back(t);
		}
		// cout << "Gather args returns: " << v << "\n";
	}
	if (formal_args.size() == 0) {
		t = arg_token(tokens, pos, get_more, false);
		if (t.get_code() != ')') {
				Error::error(E_ERR, "macro [" + name + "]: close bracket expected for function-like macro");
			return false;
		}
	}
	return (true);
}


// Return s with all \ and " characters \ escaped
static string
escape(const string& s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		switch (*i) {
		case '\\':
		case '"':
			r += '\\';
			// FALTHROUGH
		default:
			r += *i;
		}
	return (r);
}

/*
 * Convert a list of tokens into a string as specified by the # operator
 * Multiple spaces are converted to a single space, \ and " are
 * escaped
 */
static Ptoken
stringize(const listPtoken& ts)
{
	string res;
	listPtoken::const_iterator pi;
	bool seen_space = true;		// To delete leading spaces

	for (pi = ts.begin(); pi != ts.end(); pi++) {
		switch ((*pi).get_code()) {
		case '\n':
		case SPACE:
			if (seen_space)
				continue;
			else
				seen_space = true;
			res += ' ';
			break;
		case STRING_LITERAL:
			seen_space = false;
			res += "\\\"" + escape((*pi).get_val()) + "\\\"";
			break;
		case CHAR_LITERAL:
			seen_space = false;
			res += '\'' + escape((*pi).get_val()) + '\'';
			break;
		default:
			seen_space = false;
			res += escape((*pi).get_val());
			break;
		}
	}
	// Remove trailing spaces
	string::reverse_iterator i;
	int sublen = res.length();
	for (i = res.rbegin(); i != res.rend() && *i == ' '; i++)
		sublen--;
	return (Ptoken(STRING_LITERAL, res.substr(0, sublen)));
}


/*
 * Return true if if macro-replacement of *p occuring within v is allowed.
 * According to ANSI 3.8.3.1 p. 91
 * macro replacement is not performed when the argument is preceded by # or ## 
 * or followed by ##.
 * These rules do not take into account space tokens.
 */
bool
macro_replacement_allowed(const dequePtoken& v, dequePtoken::const_iterator p)
{
	dequePtoken::const_iterator i;

	// Check previous first non-white token
	for (i = p; i != v.begin(); ) {
		i--;
		if ((*i).get_code() == '#' || (*i).get_code() == CPP_CONCAT)
			return (false);
		if (!(*i).is_space())
			break;
	}

	// Check next first non-white token
	for (i = p + 1; i != v.end() && (*i).is_space(); i++)
		if ((*i).get_code() == CPP_CONCAT)
			return (false);
	return (true);
}

/*
 * Macro replace all tokens in the sequence.
 * Update tabu with the union of all macros that were used while replacing
 */
static void
macro_replace_all(listPtoken& tokens, setstring& tabu, bool get_more)
{
	listPtoken::iterator ti;
	setstring rescan_tabu(tabu);

	for (ti = tokens.begin(); ti != tokens.end(); ) {
		/*
		 * The dance with the various tabu variables is needed to
		 * ensure that while the set is updated when a macro is used
		 * inside macro_replace, this does not poison the list for
		 * further replacements done while we progress in the list.
		 */
		setstring tmptabu(rescan_tabu);
		ti = macro_replace(tokens, ti, tmptabu, get_more);
		setstring oldtabu(tabu);
		tabu.clear();
		set_union(oldtabu.begin(), oldtabu.end(), tmptabu.begin(), tmptabu.end(), inserter(tabu, tabu.begin()));
	}
}

/*
 * Check for macro at token position pos and possibly expand it  
 * If a macro is expanded, pos is invalidated and replaced with the replacement 
 * macro value.
 * Macros that are members of the tabu set are not expanded to avoid
 * infinite recursion.
 * If get_more is true, more data can be retrieved from Pltoken::get_next
 * Return the first position in tokens sequence that was not 
 * examined or replaced.
 */
listPtoken::iterator
macro_replace(listPtoken& tokens, listPtoken::iterator pos, setstring& tabu, bool get_more)
{
	mapMacro::const_iterator mi;
	const string name = (*pos).get_val();
	//cout << "macro_replace " << name << "\n";
	if ((mi = Pdtoken::macros.find(name)) == Pdtoken::macros.end() || tabu.find(name) != tabu.end())
		return (++pos);
	// cout << "replacing for " << name << "\n";
	unify(*pos, (*mi).second.name_token);
	listPtoken::iterator expand_start = pos;
	expand_start++;
	tokens.erase(pos);
	pos = expand_start;
	const Macro& m = (*mi).second;
	if (m.is_function) {
		mapArgval args;			// Map from formal name to value
		bool do_stringize;
		setstring argtabu(tabu);

		expand_start = pos;
		if (!gather_args(name, tokens, pos, m.formal_args, args, get_more))
			return (pos);
		tokens.erase(expand_start, pos);
		dequePtoken::const_iterator i;
		// Substitute with macro's replacement value
		for(i = m.value.begin(); i != m.value.end(); i++) {
			// Is it a stringizing operator ?
			if ((*i).get_code() == '#') {
				if (i + 1 == m.value.end()) {
					Error::error(E_ERR,  "Application of macro \"" + name + "\": operator # at end of macro pattern");
					return (pos);
				}
				do_stringize = true;
				// Advance to next non-space
				do {
					i++;
				} while ((*i).is_space());
			} else
				do_stringize = false;
			mapArgval::const_iterator ai;
			// Is it a formal argument?
			if ((ai = args.find((*i).get_val())) != args.end()) {
				if (macro_replacement_allowed(m.value, i)) {
					// Allowed, macro replace the parameter
					// in temporary var arg, and
					// copy that back to the main
					listPtoken arg((*ai).second.begin(), (*ai).second.end());
					// cout << "Arg macro:" << arg << "---\n";
					// See comment in macro_replace_all
					setstring tmptabu(argtabu);
					macro_replace_all(arg, tmptabu, false);
					setstring oldtabu(tabu);
					tabu.clear();
					set_union(oldtabu.begin(), oldtabu.end(), tmptabu.begin(), tmptabu.end(), inserter(tabu, tabu.begin()));
					// cout << "Arg macro result:" << arg << "---\n";
					copy(arg.begin(), arg.end(), inserter(tokens, pos));
				} else if (do_stringize)
					tokens.insert(pos, stringize((*ai).second));
				else
					copy((*ai).second.begin(), (*ai).second.end(), inserter(tokens, pos));
			} else {
				// Not a formal argument, plain replacement
				// Check for misplaced # operator (3.8.3.2)
				if (do_stringize)
					Error::error(E_WARN, "Application of macro \"" + name + "\": operator # only valid before macro parameters");
				tokens.insert(pos, *i);
			}
		}
	} else {
		// Object-like macro
		tokens.insert(pos, m.value.begin(), m.value.end());
	}

	// Check and apply CPP_CONCAT (ANSI 3.8.3.3)
	listPtoken::iterator ti, next;
	for (ti = tokens.begin(); ti != tokens.end() && ti != pos; ti = next) {
		if ((*ti).get_code() == CPP_CONCAT && ti != tokens.begin()) {
			listPtoken::iterator left = tokens.end();
			listPtoken::iterator right = tokens.end();
			listPtoken::iterator i;

			// Find left non-space operand
			for (i = ti; i != tokens.begin(); ) {
				i--;
				if (!(*i).is_space()) {
					left = i;
					break;
				}
			}
			// Find right non-space operand
			for (i = ti;; ) {
				i++;
				if (i == tokens.end() || i == pos)
					break;
				if (!(*i).is_space()) {
					right = i;
					break;
				}
			}
			if (left != tokens.end() && right != tokens.end() && right != pos) {
				next = right;
				next++;
				Tchar::push_input(*left);
				Tchar::push_input(*(right));
				Tchar::rewind_input();
				tokens.erase(left, next);
				for (;;) {
					Pltoken t;
					t.template getnext<Tchar>();
					if (t.get_code() == EOF)
						break;
					tokens.insert(next, t);
				}
			} else {
				Error::error(E_ERR, "Missing operands for ## operator");
			}
		} else {
			next = ti;
			next++;
		}
	}
	tabu.insert(name);
	// cout << "Rescan-" << name << "\n";
	macro_replace_all(tokens, tabu, get_more);
	// cout << "Rescan ends\n";
	return (pos);
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
