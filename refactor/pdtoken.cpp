/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.80 2003/07/29 20:07:23 dds Exp $
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
#include <vector>
#include <algorithm>
#if __GLIBCPP__ >= 20020816
#include <ext/functional>	// compose1
#else
#include <functional>		// compose1
#endif
#include <cassert>
#include <cstdlib>		// strtol

#include <errno.h>
#include <unistd.h>		// chdir

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
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
#include "type.h"		// stab.h
#include "stab.h"		// Block::enter()

#if __GLIBCPP__ >= 20020816
using namespace __gnu_cxx;	// STL extensions
#endif

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
	listPtoken::iterator dummy;

expand_get:
	if (!expand.empty()) {
		Pdtoken n(expand.front());
		*this = n;
		expand.pop_front();
		/*
		 * MSC extension: macros expand into a //
		 * sequence which is then treated as a comment!
		 * Search in MS headers for /##/ to see where it is used.
		 */
		if (n.get_code() == '/' && !expand.empty() && expand.front().get_code() == '/') {
			if (DP())
				cout << "Handling expanded C++ comment\n";
			while (!expand.empty())
				expand.pop_front();
			eat_to_eol();
			goto again;
		}
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
			/*
			 * @error
			 * The processing of code within a <code>#if</code>
			 * block reached the end of file, without a
			 * corresponding
			 * <code>#endif</code> /
			 * <code>#else</code> /
			 * <code>#elif</code> directive.
			 */
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
		extern bool parse_yacc_defs;
		if (parse_yacc_defs) {
			/* 
			 * We do not want the preprocessor to do macro processing 
			 * outside the context of C code in yacc files, since 
			 * terminals are often defined as numbers using macros.
			 */
			*this = t;
			break;
		}
		expand.push_front(t);
		tabu.clear();
		dummy = expand.begin();
		macro_replace(expand, expand.begin(), tabu, true, false, dummy);
		goto expand_get;
		// FALLTRHOUGH
	default:
		*this = t;
		break;
	case EOF:
		if (!iftaken.empty()) {
			Error::error(E_ERR, "EOF while processing #if directive");
			while (!iftaken.empty())
				iftaken.pop();
		}
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
		if (DP())
			cout << "yylval = " << eval_lval << "\n";
		if (*endptr == 0 || *endptr == 'l' || *endptr =='L' ||
		    *endptr == 'u' || *endptr == 'U')
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
 * -. Macro-expand sequence (ignoring the defined operator arguments)
 * -. Process defined operator
 * -. Replace all identifiers with 0
 * -. Parse and evaluate sequence
 */
static int
eval()
{
	extern int eval_parse();
	Pltoken t;

	// Read eval_tokens
	eval_tokens.clear();
	do {
		t.template getnext<Fchar>();
		eval_tokens.push_back(t);
	} while (t.get_code() != EOF && t.get_code() != '\n');

	if (DP()) {
		cout << "Tokens after reading:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}

	// Macro replace
	setstring tabu;
	macro_replace_all(eval_tokens, eval_tokens.end(), tabu, false, true);

	if (DP()) {
		cout << "Tokens after macro replace:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}

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
			/*
			 * @error
			 * The 
			 * <code>defined</code> operator was not followed
			 * by an identifier
			 */
			Error::error(E_ERR, "No identifier following defined operator");
			return 1;
		}
		if (need_bracket) {
			i2++;
			last = find_if(i2, eval_tokens.end(), not1(mem_fun_ref(&Ptoken::is_space)));
			if (last == eval_tokens.end() || (*last).get_code() != ')') {
					/*
					 * @error
					 * The identifier of a 
					 * <code>defined</code> operator was not
					 * followed by a closing bracket
					 */
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
		else
			Pdtoken::create_undefined_macro(*arg);
		eval_tokens.erase(i, last);
		eval_tokens.insert(last, Ptoken(PP_NUMBER, Pdtoken::macro_is_defined(mi) ? "1" : "0"));
		i = last;
	}
	
	if (DP()) {
		cout << "Tokens after defined:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}


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
		/*
		 * @error
		 * A
		 * <code>#if</code> or
		 * <code>#elif</code> expression was syntactically
		 * incorrect
		 */
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
Pdtoken::create_undefined_macro(const Ptoken &name)
{
	name.set_ec_attribute(is_undefined_macro);
	macros[name.get_val()] = Macro(name, false);
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
			/*
			 * @error
			 * The token following a
			 * <code>#ifdef</code>
			 * directive is not a legal identifier
			 */
			Error::error(E_WARN, "#ifdef argument is not an identifier");
		mapMacro::const_iterator i = macros.find(t.get_val());
		if (i == macros.end())
			// Heuristic; assume macro, even if it is not defined
			Pdtoken::create_undefined_macro(t);
		else
			unify(t, (*i).second.get_name_token());
		bool eval_res = Pdtoken::macro_is_defined(i);
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
		/*
		 * @error
		 * A
		 * <code>#elif</code>
		 * directive was found without a corresponding
		 * <code>#if</code>
		 */
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
		/*
		 * @error
		 * A
		 * <code>#else</code>
		 * directive was found without a corresponding
		 * <code>#if</code>
		 */
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
		/*
		 * @error
		 * A
		 * <code>#endif</code>
		 * directive was found without a corresponding
		 * <code>#if</code>
		 */
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
	static vectorstring::iterator next_i;

	if (skiplevel >= 1)
		return;
	Fchar::get_fileid().metrics().add_incfile();
	// Get tokens till end of line
	Pltoken::set_context(cpp_include);
	do {
		t.template getnext<Fchar>();
		tokens.push_back(t);
	} while (t.get_code() != EOF && t.get_code() != '\n');
	// Remove leading space
	tokens.erase(tokens.begin(), find_if(tokens.begin(), tokens.end(), not1(mem_fun_ref(&Ptoken::is_space))));
	if (tokens.size() == 0) {
		/*
		 * @error
		 * A
		 * <code>#include</code>
		 * directive was not followed by a filename specification
		 */
		Error::error(E_ERR, "Empty #include directive");
		return;
	}
	Ptoken f = *(tokens.begin());
	if (f.get_code() != PATHFNAME && f.get_code() != ABSFNAME) {
		// Need to macro process
		// 1. Macro replace
		setstring tabu;
		macro_replace_all(tokens, tokens.end(), tabu, false, false);
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
		/*
		 * @error
		 * A
		 * <code>#include</code>
		 * directive was not followed by a legal filename specification
		 */
		Error::error(E_ERR, "Invalid #include syntax");
		if (DP()) cout << f;
		return;
	}

	if (f.get_code() == ABSFNAME)
		if (can_open(f.get_val())) {
			Fchar::push_input(f.get_val());
			return;
		}
	vectorstring::iterator i;
	for (i = next ? next_i : include_path.begin(); i != include_path.end(); i++) {
		string fname = *i + "/" + f.get_val();
		if (DP()) cout << "Try open " << fname << "\n";
		if (can_open(fname)) {
			Fchar::push_input(fname);
			next_i = ++i;
			return;
		}
	}
	/*
	 * @error
	 * The specified include file could not be opened
	 */
	Error::error(E_ERR, "Unable to open include file " + f.get_val());
}

void
Pdtoken::process_define()
{
	string name;
	typedef map <string, Token> mapToken;	// To unify args with body
	mapToken args;
	Pltoken t;

	if (skiplevel >= 1)
		return;
	Pltoken::set_context(cpp_define);
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		/*
		 * @error
		 * The macro name specified in a 
		 * <code>#define</code> or
		 * <code>#undef</code>
		 * directive is not a valid identifier
		 */
		Error::error(E_ERR, "Invalid macro name");
		eat_to_eol();
		return;
	}
	t.set_ec_attribute(is_macro);
	Pltoken nametok = t;
	Macro m(t, true);
	name = t.get_val();
	t.template getnext<Fchar>();	// Space is significant: a(x) vs a (x)
	m.set_is_function(false);
	if (t.get_code() == '(') {
		// Function-like macro
		m.set_is_function(true);
		m.set_is_vararg(false);
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != ')') {
			// Formal args follow; gather them
			for (;;) {
				if (t.get_code() != IDENTIFIER) {
					/*
					 * @error
					 * A macro parameter name specified in a 
					 * <code>#define</code>
					 * directive is not a valid identifier
					 */
					Error::error(E_ERR, "Invalid macro parameter name");
					eat_to_eol();
					return;
				}
				t.set_ec_attribute(is_macroarg);
				args[t.get_val()] = t;
				m.form_args_push_back(t);
				t.template getnext_nospc<Fchar>();
				if (t.get_code() == ')') {
					t.template getnext<Fchar>();
					break;
				}
				if (t.get_code() == ELLIPSIS) {
					m.set_is_vararg(true);
					t.template getnext_nospc<Fchar>();
					if (t.get_code() != ')') {
						/*
						 * @error
						 * The ellipsis vararg specification of a
						 * <code>#define</code> macro
						 * must be the last formal argument
						 */
						Error::error(E_ERR, "Invalid vararg specification");
						eat_to_eol();
						return;
					}
					t.template getnext<Fchar>();
					break;
				}
				if (t.get_code() != ',') {
					/*
					 * @error
					 * The formal parameters in a
					 * <code>#define</code> macro
					 * definition are not separated
					 * by commas
					 */
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
		if (t.get_code() == IDENTIFIER && (i = args.find(t.get_val())) != args.end())
			unify(t, (*i).second);
		t.template getnext<Fchar>();
	}
	m.value_rtrim();

	// Check that the new macro is undefined or not different from an older definition
	mapMacro::const_iterator i = macros.find(name);
	if (i != macros.end())
		if ((*i).second.get_is_defined() && (*i).second != m) {
			/*
			 * @error
			 * A defined macro can be redefined only if the
			 * two definitions are exactly the same
			 */
			Error::error(E_WARN, "Duplicate (different) macro definition of macro " + name);
			if (DP()) cout << (*i).second;
		} else
			unify(nametok, (*i).second.get_name_token());
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
		/*
		 * @error
		 * A
		 * <code>#line</code>
		 * directive was found.
		 * This signifies that the source file is automatically
		 * generated and refactoring changes on that file may
		 * be lost in the future.
		 * References to the original source file are ignored.
		 */
		Error::error(E_WARN, "Processing automatically generated file; #line directive ignored.");
		Error::error(E_WARN, "(further #line warning messages supressed)");
		supress = 1;
	}
	eat_to_eol();
}

void
Pdtoken::process_error(enum e_error_level e)
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
	Error::error(e, msg);
}

void
Pdtoken::process_pragma()
{
	static stack <string> dirstack;

	if (skiplevel >= 1)
		return;
	Pltoken t;

	if (skiplevel >= 1)
		return;
	t.template getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		eat_to_eol();
		return;
	}
	if (t.get_val() == "sync") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma sync</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma sync: string expected");
			eat_to_eol();
			return;
		}
		string fname = t.get_val();
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != PP_NUMBER) {
			/*
			 * @error
			 * The filename of the 
			 * <code>#pragma sync</code>
			 * CScout-specific directive was not followed by an
			 * integer
			 */
			Error::error(E_ERR, "#pragma sync: integer expected");
			eat_to_eol();
			return;
		}
		char *endptr;
		unsigned long offset = strtoul(t.get_val().c_str(), &endptr, 0);
		// XXX now do the work
	} else if (t.get_val() == "nosync") {
		// XXX now do the work
	} else if (t.get_val() == "includepath") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma includepath</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma includepath: string expected");
			eat_to_eol();
			return;
		}
		Pdtoken::add_include(t.get_val());
		if (DP()) cout << "Include path " << t.get_val() << "\n";
	} else if (t.get_val() == "echo") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma echo</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma echo: string expected");
			eat_to_eol();
			return;
		}
		string s = t.get_val();
		for (string::const_iterator i = s.begin(); i != s.end();)
			cout << unescape_char(s, i);
	} else if (t.get_val() == "project") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma project</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma project: string expected");
			eat_to_eol();
			return;
		}
		Project::set_current_project(t.get_val());
	} else if (t.get_val() == "readonly") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma readonly</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma readonly: string expected");
			eat_to_eol();
			return;
		}
		Fileid fi = Fileid(t.get_val());
		fi.set_readonly(true);
	} else if (t.get_val() == "process") {
		int parse_parse();
		void garbage_collect(Fileid fi);

		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma process</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma process: string expected");
			eat_to_eol();
			return;
		}
		Fchar::push_input(t.get_val());
		Fchar::lock_stack();
		if (parse_parse() != 0)
			exit(1);
		garbage_collect(Fileid(t.get_val()));
		Fchar::unlock_stack();
	} else if (t.get_val() == "pushd") {
		char buff[4096];

		if (getcwd(buff, sizeof(buff)) == NULL)
			/*
			 * @error
			 * The call to <code>getcwd</code> failed while
			 * processing the 
			 * <code>#pragma pushd</code>
			 * CScout-specific directive
			 */
			Error::error(E_FATAL, "#pragma pushd: unable to get current directory: " + string(strerror(errno)));
		dirstack.push(buff);
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma pushd</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma pushd: string expected");
			eat_to_eol();
			return;
		}
		if (chdir(t.get_val().c_str()) != 0)
			Error::error(E_FATAL, "chdir " + t.get_val() + ": " + string(strerror(errno)));
	} else if (t.get_val() == "popd") {
		if (dirstack.empty()) {
			/*
			 * @error
			 * The
			 * <code>#pragma popd</code>
			 * CScout-specific directive was performed on an
			 * empty directory stack
			 */
			Error::error(E_ERR, "popd: directory stack empty");
			eat_to_eol();
			return;
		}
		if (chdir(dirstack.top().c_str()) != 0)
			Error::error(E_FATAL, "popd: " + dirstack.top() + ": " + string(strerror(errno)));
		dirstack.pop();
	} else if (t.get_val() == "clear_include")
		Pdtoken::clear_include();
	else if (t.get_val() == "clear_defines")
		Pdtoken::macros_clear();
	else if (t.get_val() == "ro_prefix") {
		t.template getnext_nospc<Fchar>();
		if (t.get_code() != STRING_LITERAL) {
			/*
			 * @error
			 * The
			 * <code>#pragma ro_prefix</code>
			 * CScout-specific directive was not followed by a 
			 * string
			 */
			Error::error(E_ERR, "#pragma ro_prefix: string expected");
			eat_to_eol();
			return;
		}
		Fileid::add_ro_prefix(t.get_val());
	} else if (t.get_val() == "block_enter")
		Block::enter();
	else if (t.get_val() == "block_exit")
		Block::exit();
	eat_to_eol();
}

void
Pdtoken::process_directive()
{
	Pltoken t;

	Fchar::get_fileid().metrics().add_ppdirective();
	t.template getnext_nospc<Fchar>();
	if (t.get_code() == '\n')		// Empty directive
		return;
	if (DP())
		cout << "Directive: " << t << "\n";
	if (t.get_code() != IDENTIFIER) {
		/*
		 * @error
		 * An invalid preprocessor directive was found.
		 * The directive did not match an identifier
		 */
		Error::error(E_ERR, "Invalid preprocessor directive");
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
		process_error(E_ERR);
	else if (t.get_val() == "warning")	// GCC extension
		process_error(E_WARN);
	else if (t.get_val() == "pragma")
		process_pragma();
	else
		/*
		 * @error
		 * An unkown preprocessor directive was found.
		 */
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
