/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: macro.cpp,v 1.10 2002/09/17 10:53:02 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <algorithm>
#include <functional>		// ptr_fun
#include <cassert>
#include <cstdlib>		// strtol
#include <cctype>		// isspace

#include "cpp.h"
#include "debug.h"
#include "metrics.h"
#include "attr.h"
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
#include "tchar.h"
#include "ctoken.h"

void macro_replace_all(listPtoken& tokens, listPtoken::iterator end, setstring& tabu, bool get_more);

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
	assert (t.get_code() == '(');
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
		if (DP()) cout << "Gather args returns: " << v << "\n";
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
			res += (*pi).get_val();
			break;
		}
	}
	// Remove trailing spaces
	res.erase((find_if(res.rbegin(), res.rend(), not1(ptr_fun<int, int>(isspace)))).base(), res.end());
	return (Ptoken(STRING_LITERAL, res));
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
 * Macro replace all tokens in the sequence from tokens.begin() up to the 
 * "end" iterator
 */
void
macro_replace_all(listPtoken& tokens, listPtoken::iterator end, setstring& tabu, bool get_more)
{
	listPtoken::iterator ti;
	setstring rescan_tabu(tabu);

	if (DP()) cout << "Enter replace_all\n";
	for (ti = tokens.begin(); ti != end; ) {
		if ((*ti).get_code() == IDENTIFIER)
			ti = macro_replace(tokens, ti, tabu, get_more);
		else
			ti++;
	}
	if (DP()) cout << "Exit replace_all\n";
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
macro_replace(listPtoken& tokens, listPtoken::iterator pos, setstring tabu, bool get_more)
{
	mapMacro::const_iterator mi;
	const string name = (*pos).get_val();
	#ifdef ndef
	cout << "macro_replace: [" << name << "] tabu: ";
	for (setstring::const_iterator si = tabu.begin(); si != tabu.end(); si++)
		cout << *si << " ";
	cout << "\n";
	#endif
	if ((mi = Pdtoken::macros_find(name)) == Pdtoken::macros_end() || !(*pos).can_replace())
		return (++pos);
	if (tabu.find(name) != tabu.end()) {
		(*pos).set_nonreplaced();
		return (++pos);
	}
	const Macro& m = mi->second;
	if (m.is_function) {
		// Peek for a left bracket, if not found this is not a macro
		listPtoken::iterator peek = pos;
		peek++;
		while (peek != tokens.end() && (*peek).is_space())
			peek++;
		if (peek == tokens.end()) {
			if (get_more) {
				Pltoken t;
				do {
					t.template getnext<Fchar>();
					tokens.push_back(t);
				} while (t.get_code() != EOF && t.is_space());
				if (t.get_code() != '(')
					return (++pos);
			} else
				return (++pos);
		} else if ((*peek).get_code() != '(')
			return (++pos);
	}
	if (DP()) cout << "replacing for " << name << "\n";
	unify(*pos, (*mi).second.name_token);
	listPtoken::iterator expand_start = pos;
	expand_start++;
	tokens.erase(pos);
	pos = expand_start;
	if (m.is_function) {
		mapArgval args;			// Map from formal name to value
		bool do_stringize;

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
					if (DP()) cout << "Arg macro:" << arg << "---\n";
					macro_replace_all(arg, arg.end(), tabu, false);
					if (DP()) cout << "Arg macro result:" << arg << "---\n";
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
			listPtoken::iterator left = ti;
			listPtoken::iterator right = pos;
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
			Tchar::clear();
			// See if non-empty left operand
			if (left != ti)
				Tchar::push_input(*left);
			next = right;
			// See if non-empty right operand
			if (right != pos) {
				Tchar::push_input(*(right));
				next++;
			}
			if (DP()) cout << "concat A:" << *left << "B: " << *right << "\n";
			Tchar::rewind_input();
			tokens.erase(left, next);
			for (;;) {
				Pltoken t;
				t.template getnext<Tchar>();
				if (t.get_code() == EOF)
					break;
				if (DP()) cout << "Result: " << t ;
				tokens.insert(next, t);
			}
		} else {
			next = ti;
			next++;
		}
	}
	tabu.insert(name);
	if (DP()) cout << "Rescan-" << name << "\n";
	macro_replace_all(tokens, pos, tabu, get_more);
	if (DP()) cout << "Rescan ends\n";
	return (pos);
}

static inline bool
space_eq(Ptoken& a, Ptoken& b)
{
	return a.is_space() && b.is_space();
}

// True if two macro definitions are the same
bool
operator ==(const Macro& a, const Macro& b)
{
	if (a.is_function != b.is_function || a.formal_args != b.formal_args)
		return false;
	
	// Remove consecutive spaces
	dequePtoken va(a.value);
	dequePtoken vb(b.value);
	va.erase(unique(va.begin(), va.end(), space_eq), va.end());
	vb.erase(unique(vb.begin(), vb.end(), space_eq), vb.end());
	return (va == vb);
}

// Remove trailing whitespace
void
Macro::value_rtrim()
{
	// Took me three hours to arrive at
	value.erase((find_if(value.rbegin(), value.rend(), not1(mem_fun_ref(&Ptoken::is_space)))).base(), value.end());
}

ostream&
operator<<(ostream& o,const Macro &m)
{
	o << m.name_token.get_val();
	if (m.is_function) {
		o << "(";
		for (dequePtoken::const_iterator i = m.formal_args.begin(); i != m.formal_args.end(); i++)
			o << (*i).get_val() << ", ";
		o << ")";
	}
	o << " ";
	for (dequePtoken::const_iterator i = m.value.begin(); i != m.value.end(); i++)
		o << (*i).get_val();
	o << "\n";
	return (o);
}
