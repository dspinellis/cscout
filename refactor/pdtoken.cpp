/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.45 2001/09/03 10:15:26 dds Exp $
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
#include <functional>
#include <cassert>
#include <cstdlib>		// strtol

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
#include "tchar.h"
#include "ctoken.h"

bool Pdtoken::at_bol = true;
listPtoken Pdtoken::expand;
mapMacro Pdtoken::macros;		// Defined macros
stackbool Pdtoken::iftaken;		// Taken #ifs
vectorstring Pdtoken::include_path;	// Files in include path
int Pdtoken::skiplevel = 0;		// Level of enclosing #ifs when skipping


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
	if (skiplevel) {
		if (t.get_code() == '\n')
			at_bol = true;
		else if (t.get_code() == EOF) {
			Error::error(E_ERR, "EOF while processing #if directive");
			*this = t;
			return;
		}
		goto again;
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
	case EOF:
		if (!iftaken.empty())
			Error::error(E_ERR, "EOF while processing #if directive");
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
 * Lexical analyser for #if expressions
 */
static listPtoken::iterator eval_ptr;
static listPtoken eval_tokens;
long eval_result;

int
eval_lex_real()
{
	extern long eval_lval;
	const char *num;
	char *endptr;
	Ptoken t;
	int code;
again:
	if (eval_ptr == eval_tokens.end())
		return 0;
	switch ((t = (*eval_ptr++)).get_code()) {
	case '\n':
	case SPACE:
		goto again;
	case PP_NUMBER:
		num = t.get_val().c_str();
		eval_lval = strtol(num, &endptr, 0);
		if (*endptr == 0 || *endptr == 'l' || *endptr =='L')
			return (INT_CONST);
		else
			return (FLOAT_CONST);	// Should be flagged as error
	case CHAR_LITERAL:
		{
		const string& s = t.get_val();
		string::const_iterator si;

		//cout << "char:[" << s << "]\n";
		si = s.begin();
		eval_lval = unescape_char(s, si);
		if (si != s.end())
			Error::error(E_ERR, "Illegal characters in character escape sequence");
		return (INT_CONST);
		}
	default:
		return (t.get_code());
	}
}

// Used for debugging
int
eval_lex()
{
	int l = eval_lex_real();
	if (DP())
		cout << "Eval lex returns " << Token(l) << "\n";
	return (l);
}

/*
 * Read tokens comprising a cpp expression up to the newline and return
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
	extern eval_parse();
	Pltoken t;

	// Read eval_tokens
	eval_tokens.clear();
	do {
		t.template getnext<Fchar>();
		eval_tokens.push_back(t);
	} while (t.get_code() != EOF && t.get_code() != '\n');

	//cout << "Tokens after reading:\n";
	//copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));

	// Process the "defined" operator
	listPtoken::iterator i, arg, last, i2;
	for (i = eval_tokens.begin(); 
	     (i = i2 = find_if(i, eval_tokens.end(), compose1(bind2nd(equal_to<string>(),"defined"), mem_fun_ref(&Ptoken::get_val)))) != eval_tokens.end(); ) {
	     	bool need_bracket = false;
		i2++;
		arg = i2 = find_if(i2, eval_tokens.end(), not1(mem_fun_ref(&Ptoken::is_space)));
		if (arg != eval_tokens.end() && (*arg).get_code() == '(') {
			i2++;
			arg = i2 = find_if(i2, eval_tokens.end(), not1(mem_fun_ref(&Ptoken::is_space)));
			need_bracket = true;
		}
		if (arg == eval_tokens.end() || (*arg).get_code() != IDENTIFIER) {
			Error::error(E_ERR, "No identifier following defined operator");
			return 1;
		}
		if (need_bracket) {
			i2++;
			last = find_if(i2, eval_tokens.end(), not1(mem_fun_ref(&Ptoken::is_space)));
			if (last == eval_tokens.end() || (*last).get_code() != ')') {
				Error::error(E_ERR, "Missing close bracket in defined operator");
				return 1;
			}
		} else
			last = arg;
		last++;
		// We are about to erase it
		string val = (*arg).get_val();
		if (DP()) cout << "val:" << val << "\n";
		mapMacro::const_iterator mi = Pdtoken::macros_find(val);
		if (mi != Pdtoken::macros_end())
			unify(*arg, (*mi).second.get_name_token());
		eval_tokens.erase(i, last);
		eval_tokens.insert(last, Ptoken(PP_NUMBER, mi == Pdtoken::macros_end() ? "0" : "1"));
		i = last;
	}
	//cout << "Tokens after defined:\n";
	//copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));

	// Macro replace
	setstring tabu;
	macro_replace_all(eval_tokens, eval_tokens.end(), tabu, false);

	// Change remaining identifiers to 0
	for (i = eval_tokens.begin(); 
	     (i = find_if(i, eval_tokens.end(), compose1(bind2nd(equal_to<int>(),IDENTIFIER), mem_fun_ref(&Ptoken::get_code)))) != eval_tokens.end(); )
	     	*i = Ptoken(PP_NUMBER, "0");
	eval_ptr = eval_tokens.begin();
	if (DP()) {
		cout << "Tokens before parsing:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}
	if (eval_parse() != 0) {
		Error::error(E_ERR, "Syntax error in preprocessor expression");
		return 1;
	}
	if (DP()) cout << "Eval returns: " << eval_result << "\n";
	return (eval_result);
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
 * Skipping is performed by setting skiplevel=1 and looking for a 
 * #e* when skiplevel == 1.
 * While skiplevel > 0 each #if* results in skiplevel++, each #endif in 
 * skiplevel--.
 * Skiplevel handling:
 * 0 normal processing
 * >= 1 skipping where value is the number of enclosing #if blocks
 */
void
Pdtoken::process_if()
{
	if (skiplevel)
		skiplevel++;
	else {
		bool eval_res = eval();
		iftaken.push(eval_res);
		skiplevel = eval_res ? 0 : 1;
	}
}

void
Pdtoken::process_ifdef(bool isndef)
{
	if (skiplevel)
		skiplevel++;
	else {
		Pltoken t;

		t.template getnext_nospc<Fchar>();
		if (t.get_code() != IDENTIFIER)
			Error::error(E_WARN, "#ifdef argument is not an identifier");
		bool eval_res = Pdtoken::macros_find(t.get_val()) != Pdtoken::macros_end();
		if (isndef)
			eval_res = !eval_res;
		iftaken.push(eval_res);
		skiplevel = eval_res ? 0 : 1;
	}
}


void
Pdtoken::process_elif()
{
	if (iftaken.empty()) {
		Error::error(E_ERR, "Unbalanced #elif");
		eat_to_eol();
		return;
	}
	if (skiplevel > 1)
		return;
	if (iftaken.top())
		skiplevel = 1;
	else {
		iftaken.pop();
		skiplevel = 0;
		process_if();
	}
}

void
Pdtoken::process_else()
{
	if (iftaken.empty()) {
		Error::error(E_ERR, "Unbalanced #else");
		eat_to_eol();
		return;
	}
	if (skiplevel > 1)
		return;
	if (iftaken.top()) {
		skiplevel = 1;
		return;
	}
	skiplevel = 0;
	eat_to_eol();
}

void
Pdtoken::process_endif()
{
	if (iftaken.empty()) {
		Error::error(E_ERR, "Unbalanced #endif");
		eat_to_eol();
		return;
	}
	if (skiplevel <= 1)
		iftaken.pop();
	if (skiplevel >= 1)
		skiplevel--;
	eat_to_eol();
}

// Return true if we can open a given file
static bool
can_open(const string& s)
{
	ifstream in;
	bool ret;
	in.open(s.c_str());
	if (!in.fail()) {
		in.close();
		return (true);
	} else
		return (false);
}

/*
 * When next is true we start scanning the include path from where we left
 * off.  This is a crude hack to approximate the unkown semantics of the
 * gcc include_next command.
 */
void
Pdtoken::process_include(bool next)
{
	Pltoken t;
	listPtoken tokens;
	static vectorstring::const_iterator next_i;

	if (skiplevel >= 1)
		return;
	// Get tokens till end of line
	Pltoken::set_context(cpp_include);
	bool start = true;
	do {
		t.template getnext<Fchar>();
		tokens.push_back(t);
	} while (t.get_code() != EOF && t.get_code() != '\n');
	// Remove leading space
	tokens.erase(tokens.begin(), find_if(tokens.begin(), tokens.end(), not1(mem_fun_ref(&Ptoken::is_space))));
	if (tokens.size() == 0) {
		Error::error(E_ERR, "Empty #include directive");
		return;
	}
	Ptoken f = *(tokens.begin());
	if (f.get_code() != PATHFNAME && f.get_code() != ABSFNAME) {
		// Need to macro process
		// 1. Macro replace
		setstring tabu;
		macro_replace_all(tokens, tokens.end(), tabu, false);
		if (DP()) {
			cout << "Replaced after macro :\n";
			copy(tokens.begin(), tokens.end(), ostream_iterator<Ptoken>(cout));
		}
		// 2. Rescan through Tchar
		Tchar::clear();
		for (listPtoken::const_iterator i = tokens.begin(); i != tokens.end(); i++)
			Tchar::push_input(*i);
		Tchar::rewind_input();
		Pltoken::set_context(cpp_include);
		t.template getnext<Tchar>();
		f = t;
	}
	if (f.get_code() == EOF) {
		Error::error(E_ERR, "Empty #include directive");
		return;
	}

	if (f.get_code() != PATHFNAME && f.get_code() != ABSFNAME) {
		Error::error(E_ERR, "Invalid #include syntax");
		if (DP()) cout << f;
		return;
	}

	if (f.get_code() == ABSFNAME)
		if (can_open(f.get_val())) {
			Fchar::push_input(f.get_val());
			return;
		}
	vectorstring::const_iterator i;
	for (i = next ? next_i : include_path.begin(); i != include_path.end(); i++) {
		string fname = *i + "/" + f.get_val();
		if (DP()) cout << "Try open " << fname << "\n";
		if (can_open(fname)) {
			Fchar::push_input(fname);
			next_i = ++i;
			return;
		}
	}
	Error::error(E_ERR, "Unable to open include file " + f.get_val());
}

void
Pdtoken::process_define()
{
	Macro m;
	string name;
	typedef map <string, Token> mapToken;	// To unify args with body
	mapToken args;
	Pltoken t;

	if (skiplevel >= 1)
		return;
	Pltoken::set_context(cpp_define);
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		Error::error(E_ERR, "Invalid macro name");
		eat_to_eol();
		return;
	}
	m.set_name_token(t);
	name = t.get_val();
	t.template getnext<Fchar>();	// Space is significant: a(x) vs a (x)
	m.set_is_function(false);
	if (t.get_code() == '(') {
		// Function-like macro
		m.set_is_function(true);
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != ')') {
			// Formal args follow; gather them
			for (;;) {
				if (t.get_code() != IDENTIFIER) {
					Error::error(E_ERR, "Invalid macro parameter name");
					eat_to_eol();
					return;
				}
				args[t.get_val()] = t;
				m.form_args_push_back(t);
				t.template getnext_nospc<Fchar>();
				if (t.get_code() == ')') {
					t.template getnext<Fchar>();
					break;
				}
				if (t.get_code() != ',') {
					Error::error(E_ERR, "Invalid macro parameter punctuation");
					eat_to_eol();
					return;
				}
				t.template getnext_nospc<Fchar>();
			}
		} else
			t.template getnext<Fchar>();
	}
	if (DP()) cout << "Body starts with " << t;
	// Continue gathering macro body
	for (bool lead = true; t.get_code() != '\n';) {
		// Ignore leading whitespace (not newlines)
		if (lead && t.is_space()) {
			if (t.get_code() == SPACE)
				t.template getnext<Fchar>();
			if (t.get_code() == '\n')
				break;
			continue;
		}
		lead = false;
		m.value_push_back(t);
		mapToken::const_iterator i;
		if ((i = args.find(t.get_val())) != args.end())
			unify(t, (*i).second);
		t.template getnext<Fchar>();
	}
	m.value_rtrim();

	// Check that the new macro is not different from an older definition
	mapMacro::const_iterator i = macros.find(name);
	if (i != macros.end() && (*i).second != m) {
		Error::error(E_WARN, "Duplicate (different) macro definition of macro " + name);
		if (DP()) cout << (*i).second;
	}
	macros[name] = m;
	if (DP()) cout << "Macro define " << m;
}

void
Pdtoken::process_undef()
{
	Pltoken t;

	if (skiplevel >= 1)
		return;
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		Error::error(E_ERR, "Invalid macro name");
		eat_to_eol();
		return;
	}
	mapMacro::iterator mi;
	if ((mi = Pdtoken::macros.find(t.get_val())) != Pdtoken::macros.end()) {
		unify(t, (*mi).second.get_name_token());
		Pdtoken::macros.erase(mi);
	}
	eat_to_eol();
}

void
Pdtoken::process_line()
{
	static bool supress = false;

	if (skiplevel >= 1)
		return;
	if (!supress) {
		Error::error(E_WARN, "Processing automatically generated file; #line directive ignored.");
		Error::error(E_WARN, "(further #line warning messages supressed)");
		supress = 1;
	}
	eat_to_eol();
}

void
Pdtoken::process_error()
{
	string msg;
	Pltoken t;

	if (skiplevel >= 1)
		return;
	for (;;) {
		t.template getnext<Fchar>();
		if (t.get_code() == EOF || t.get_code() == '\n')
			break;
		else
			msg += t.get_val();
	}
	Error::error(E_ERR, msg);
}

void
Pdtoken::process_pragma()
{
	if (skiplevel >= 1)
		return;
	Pltoken t;

	if (skiplevel >= 1)
		return;
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER || t.get_val() != "includepath") {
		eat_to_eol();
		return;
	}
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != STRING_LITERAL) {
		eat_to_eol();
		return;
	}
	Pdtoken::add_include(t.get_val());
	if (DP()) cout << "Include path " << t.get_val() << "\n";
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
	else if (t.get_val() == "include_next") // GCC extension
		process_include(true);
	else if (t.get_val() == "include")
		process_include(false);
	else if (t.get_val() == "if")
		process_if();
	else if (t.get_val() == "ifdef")
		process_ifdef(false);
	else if (t.get_val() == "ifndef")
		process_ifdef(true);
	else if (t.get_val() == "elif")
		process_elif();
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
