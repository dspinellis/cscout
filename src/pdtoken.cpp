/*
 * (C) Copyright 2001-2019 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * For documentation read the corresponding .h file
 *
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
#include <cstdlib>		// strtoul
#include <cstring>		// strerror

#include <errno.h>
#include <unistd.h>		// chdir

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "parse.tab.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "tchar.h"
#include "ctoken.h"
#include "eval.h"
#include "type.h"
#include "stab.h"
#include "call.h"
#include "mcall.h"
#include "os.h"
#include "ctag.h"
#include "type.h"		// stab.h
#include "stab.h"		// Block::enter()

bool Pdtoken::at_bol = true;
bool Pdtoken::output_defines = false;
PtokenSequence Pdtoken::expand;
mapMacro Pdtoken::macros;		// Defined macros
stackbool Pdtoken::iftaken;		// Taken #ifs
vectorstring Pdtoken::include_path;	// Files in include path
int Pdtoken::skiplevel = 0;		// Level of enclosing #ifs when skipping
mapMacroBody Pdtoken::macro_body_tokens;	// Tokens and the macros they belong to
// Files that must be skipped rather than included (#pragma once)
set<Fileid> Pdtoken::skipped_includes;
vectorPdtoken Pdtoken::current_line;	// Currently read line

CompiledRE Pdtoken::preprocessed_output_spec;	// Files to preprocess

bool
Pdtoken::shall_skip(Fileid fid)
{
	bool ret = skipped_includes.find(fid) != skipped_includes.end();
	if (DP())
		cout << "shall_skip(" << fid.get_path() << " = " << ret << '\n';
	return ret;
}

void
Pdtoken::getnext()
{
	getnext_expand();
	if (get_code() == '\n')
		current_line.clear();
	else
		current_line.push_back(*this);
}

void
Pdtoken::getnext_expand()
{
	Pltoken t;

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
	t.getnext<Fchar>();
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
		expand = macro_expand(expand, true, false);
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

void
Pdtoken::getnext_noexpand()
{
	Pltoken t;

again:
	t.getnext<Fchar>();
	if (at_bol) {
		if (t.get_code() == '#') {
			process_directive();
			goto again;
		} else
			at_bol = false;
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
		/* FALLTHROUGH */
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
	if (DP())
		cout << "getnext_noexpand returns " << *this << endl;
}

void
Pdtoken::getnext_noexpand_nospc()
{
	do {
		getnext_noexpand();
	} while (code == SPACE);
}


// Consume input up to (and including) the first \n
void
Pdtoken::eat_to_eol()
{
	Pltoken t;

	do {
		t.getnext<Fchar>();
	} while (t.get_code() != EOF && t.get_code() != '\n');
	Pltoken::clear_echo();
}


/*
 * Lexical analyser for #if expressions
 */
static PtokenSequence::iterator eval_ptr;
static PtokenSequence eval_tokens;
long eval_result;

int
eval_lex_real()
{
	Ptoken t;
again:
	if (eval_ptr == eval_tokens.end())
		return 0;
	switch ((t = (*eval_ptr++)).get_code()) {
	case '\n':
	case SPACE:
		goto again;
	case PP_NUMBER:
		{
			const char *num;
			char *endptr;
			string s(t.get_val());

			num = s.c_str();
			eval_lval.v.u = strtoul(num, &endptr, 0);
			if (*endptr == 0 || *endptr == 'l' || *endptr =='L' ||
			    *endptr == 'u' || *endptr == 'U') {
				eval_lval.su = e_signed;
				for (; *endptr; endptr++)
					if (*endptr == 'u' || *endptr == 'U')
						eval_lval.su = e_unsigned;
				if (DP()) {
					if (eval_lval.su == e_signed)
						cout << "yylval = (signed)" << eval_lval.v.s << endl;
					else
						cout << "yylval = (unsigned)" << eval_lval.v.u << endl;
				}
				return (INT_CONST);
			} else
				return (FLOAT_CONST);	// Should be flagged as error
		}
	case CHAR_LITERAL:
		{
		const string& s = t.get_val();
		string::const_iterator si;

		if (DP()) cout << "char:[" << s << "]\n";
		si = s.begin();
		eval_lval.v.s = unescape_char(s, si);
// Behave as the hosting environment
#if '\xff' < 0
		eval_lval.su = e_signed;
#else
		eval_lval.su = e_unsigned;
#endif
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
 * -. Replace predicates
 * -. Macro-expand sequence (ignoring the defined operator arguments)
 * -. Process defined operator
 * -. Replace all identifiers with 0
 * -. Parse and evaluate sequence
 */
static int
eval()
{
	Pltoken t;

	// Read eval_tokens
	eval_tokens.clear();
	do {
		t.getnext<Fchar>();
		eval_tokens.push_back(t);
	} while (t.get_code() != EOF && t.get_code() != '\n');

	if (DP()) {
		cout << "Tokens after reading:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}

	// Macro replace
	eval_tokens = macro_expand(eval_tokens, false, true);

	if (DP()) {
		cout << "Tokens after macro replace:\n";
		copy(eval_tokens.begin(), eval_tokens.end(), ostream_iterator<Ptoken>(cout));
	}

	// Process the "defined" operator
	PtokenSequence::iterator i, arg, last, i2;
	for (i = eval_tokens.begin();
	     (i = i2 = find_if(i, eval_tokens.end(), [](const Ptoken &t) { return t.get_val() == "defined"; })) != eval_tokens.end(); ) {
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
			Token::unify((*mi).second.get_name_token(), *arg);
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
	     (i = find_if(i, eval_tokens.end(), [](const Ptoken &t) { return t.get_code() == IDENTIFIER; })) != eval_tokens.end(); )
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
		Metrics::call_metrics(&Metrics::add_ppcond);
		bool eval_res = eval();
		iftaken.push(eval_res);
		skiplevel = eval_res ? 0 : 1;
	}
}

void
Pdtoken::create_undefined_macro(const Ptoken &name)
{
	name.set_ec_attribute(is_undefined_macro);
	mapMacro::value_type v(name.get_val(), Macro(name, false, false));
	// XXX Passing the above value directly causes a crash with
	// gcc version 3.2
	macros.insert(v);
}

void
Pdtoken::process_ifdef(bool isndef)
{
	if (skiplevel)
		skiplevel++;
	else {
		Pltoken t;

		Metrics::call_metrics(&Metrics::add_ppcond);
		t.getnext_nospc<Fchar>();
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
			Token::unify((*i).second.get_name_token(), t);
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
	Metrics::call_metrics(&Metrics::add_ppcond);
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
 * When next is true we start scanning the include path from the directory
 * following the one in which the current file was found (gcc extension).
 * See: http://www.delorie.com/gnu/docs/gcc/cpp_11.html (Wrapper headers)
 */
void
Pdtoken::process_include(bool next)
{
	Pltoken t;
	PtokenSequence tokens;

	if (skiplevel >= 1)
		return;
	Fchar::get_fileid().metrics().add_incfile();
	// Get tokens till end of line
	Pltoken::set_context(cpp_include);
	do {
		t.getnext<Fchar>();
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
		tokens = macro_expand(tokens, false, false);
		if (DP()) {
			cout << "Replaced after macro :\n";
			copy(tokens.begin(), tokens.end(), ostream_iterator<Ptoken>(cout));
		}
		// 2. Rescan through Tchar
		Tchar::clear();
		for (PtokenSequence::const_iterator i = tokens.begin(); i != tokens.end(); i++)
			Tchar::push_input(*i);
		Tchar::rewind_input();
		Pltoken::set_context(cpp_include);
		t.getnext<Tchar>();
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

	if (DP()) {
		cout << "Ready to include:\n";
		copy(tokens.begin(), tokens.end(), ostream_iterator<Ptoken>(cout));
	}
	// #include <foo.h> and #include "foo.h"
	if (is_absolute_filename(f.get_val())) {
		if (can_open(f.get_val())) {
			if (!Pdtoken::shall_skip(Fileid(f.get_val())))
				Fchar::push_input(f.get_val());
			return;
		}
	} else {
		/*
		 * #include "foo.h"
		 * Where to search is implementation specific.
		 * - Harbison and Steele recommend in the current dir (p. 45)
		 * - gcc 3.4.2 searches:
		 * 1) in the including file's dir
		 * 2) in the specified include path
		 * - Microsoft C 11.0.7922 searches:
		 * 1) in the including file's dir
		 * 2) in the current directory
		 * 3) in the specified include path
		 */
		if (f.get_code() == ABSFNAME && !next) {
			string fname(Fchar::get_dir() + "/" + f.get_val());
			if (can_open(fname)) {
				if (!Pdtoken::shall_skip(Fileid(fname)))
					Fchar::push_input(fname);
				return;
			}
		}
		vectorstring::iterator i;
		i = include_path.begin();
		if (next)
			i += Fchar::get_fileid().get_ipath_offset() + 1;
		for (; i < include_path.end(); i++) {
			string fname(*i + "/" + f.get_val());
			if (DP()) cout << "Try open " << fname << "\n";
			if (can_open(fname)) {
				if (!Pdtoken::shall_skip(Fileid(fname)))
					Fchar::push_input(fname);
				Fchar::get_fileid().set_ipath_offset(i - include_path.begin());
				return;
			}
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
	if (output_defines) {
		cout << "#define";
		Pltoken::set_echo();
	}
	Pltoken::set_context(cpp_define);
	t.getnext_nospc<Fchar>();
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
	CTag::add(t, 'd');
	t.set_ec_attribute(is_macro);
	Pltoken nametok = t;
	name = t.get_val();
	t.getnext<Fchar>();	// Space is significant: a(x) vs a (x)
	bool is_function = (t.get_code() == '(');
	Macro m(nametok, true, is_function);
	if (is_function) {
		// Function-like macro
		Metrics::call_metrics(&Metrics::add_ppfmacro);
		m.set_is_vararg(false);
		t.getnext_nospc<Fchar>();
		if (t.get_code() != ')') {
			/*
			 * Formal args follow; gather them
			 * For variable # of arguments we accept the gcc syntax:
			 * #define debug(format, args...) fprintf (stderr, format, ##args)
			 * and the ANSI C99 syntax:
			 * #define debug(format, ...) fprintf (stderr, format, __VA_ARGS__)
			 */
			for (;;) {
				// Test ELLIPSIS for C99 varargs
				if (t.get_code() != IDENTIFIER && t.get_code() != ELLIPSIS) {
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
				if (t.get_code() == IDENTIFIER) {
					t.set_ec_attribute(is_macro_arg);
					args[t.get_val()] = t;
					m.form_args_push_back(t);
					t.getnext_nospc<Fchar>();
					if (t.get_code() == ')') {
						MCall::set_current_fun(m);
						t.getnext<Fchar>();
						break;
					}
				} else {
					// C99 vararg
					Ptoken va(IDENTIFIER, "__VA_ARGS__");
					m.form_args_push_back(va);
				}
				// This applies to C99 and gcc varargs
				if (t.get_code() == ELLIPSIS) {
					m.set_is_vararg(true);
					t.getnext_nospc<Fchar>();
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
					MCall::set_current_fun(m);
					t.getnext<Fchar>();
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
				t.getnext_nospc<Fchar>();
			}
		} else {
			// Function-like macro without formal args
			MCall::set_current_fun(m);
			t.getnext<Fchar>();
		}
		MCall::set_num_args(m.get_num_args());
	} else
		Metrics::call_metrics(&Metrics::add_ppomacro);
	if (DP()) cout << "Body starts with " << t;
	// Continue gathering macro body
	for (bool lead = true; t.get_code() != '\n';) {
		// Ignore leading whitespace (not newlines)
		if (lead && t.is_space()) {
			if (t.get_code() == SPACE)
				t.getnext<Fchar>();
			if (t.get_code() == '\n')
				break;
			continue;
		}
		lead = false;
		m.value_push_back(t);
		mapToken::const_iterator i;
		if (t.get_code() == IDENTIFIER) {
			t.set_ec_attribute(is_macro_token);
			if ((i = args.find(t.get_val())) != args.end())
				Token::unify((*i).second, t);
		}
		t.getnext<Fchar>();
	}
	if (is_function)
		Call::unset_current_fun();
	m.value_rtrim();

	// Check that the new macro is undefined or not different from an older definition
	mapMacro::const_iterator i = macros.find(name);
	if (i != macros.end()) {
		if ((*i).second.get_is_defined() && (*i).second != m) {
			/*
			 * @error
			 * A defined macro can be redefined only if the
			 * two definitions are exactly the same
			 */
			Error::error(E_WARN, "Duplicate (different) macro definition of macro " + name);
			if (DP()) cout << (*i).second;
		} else
			Token::unify((*i).second.get_name_token(), nametok);
	}
	/*
	 * Add the macro in the map.  We do not use [] to avoid
	 * creating a default object.  We do not use insert,
	 * to ensure updating a previously defined object.
	 */
	mapMacro::iterator mi = macros.find(name);
	if (mi == macros.end())
		macros.insert(mapMacro::value_type(name, m));
	else
		mi->second = m;
	if (is_function)
		m.register_macro_body(macro_body_tokens);
	if (DP()) cout << "Macro define " << m;
	Pltoken::clear_echo();
}

void
Pdtoken::process_undef()
{
	Pltoken t;

	if (skiplevel >= 1)
		return;
	t.getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		Error::error(E_ERR, "Invalid macro name");
		eat_to_eol();
		return;
	}
	mapMacro::iterator mi;
	if ((mi = Pdtoken::macros.find(t.get_val())) != Pdtoken::macros.end()) {
		Token::unify((*mi).second.get_name_token(), t);
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

	// Must consume the tokens whether skipping or not, to avoid processing # as a directive
	for (;;) {
		t.getnext<Fchar>();
		if (t.get_code() == EOF || t.get_code() == '\n')
			break;
		else
			msg += t.get_val();
	}
	if (skiplevel == 0)
		Error::error(e, msg);
}

// Preprocess specified file to the standard output
static void
preprocess_to_output(const string& s)
{
	Fchar::push_input(s);
	Fchar::lock_stack();
	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		cout << t.get_c_val();
	}
	Fchar::unlock_stack();
}

void
Pdtoken::process_pragma()
{
	static stack <string> dirstack;

	if (skiplevel >= 1)
		return;
	Pltoken t;

	t.getnext_nospc<Fchar>();
	if (t.get_code() != IDENTIFIER) {
		eat_to_eol();
		return;
	}
	if (t.get_val() == "sync") {
		t.getnext_nospc<Fchar>();
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
		t.getnext_nospc<Fchar>();
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
		offset += 0;	// silence the compiler warning
		// XXX now do the work
	} else if (t.get_val() == "nosync") {
		// XXX now do the work
	} else if (t.get_val() == "once") {
		// Mark the file for skipping next time it is included.
		Fileid fid(Fchar::get_fileid());
		if (DP()) cout << "Pragma once on " << fid.get_path() << "\n";
		Pdtoken::set_skip(fid);
	} else if (t.get_val() == "includepath") {
		t.getnext_nospc<Fchar>();
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
		t.getnext_nospc<Fchar>();
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
		// Echo unless we're preprocessing to stdout
		if (!preprocessed_output_spec.isSet()) {
			string s = t.get_val();
			for (string::const_iterator i = s.begin(); i != s.end();)
				cerr << unescape_char(s, i);
		}
	} else if (t.get_val() == "project") {
		t.getnext_nospc<Fchar>();
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
		t.getnext_nospc<Fchar>();
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
		t.getnext_nospc<Fchar>();
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
		if (preprocessed_output_spec.isSet()) {
			// Skip or enable preprocessed output
			if (preprocessed_output_spec.exec( t.get_val().c_str(),
						0, NULL, 0) != REG_NOMATCH)
				preprocess_to_output(t.get_val());
		}
		if (!preprocessed_output_spec.isSet()) {
			// Normal processing
			extern int parse_parse();
			extern void garbage_collect(Fileid fi);

			Fchar::push_input(t.get_val());
			Fchar::lock_stack();
			if (parse_parse() != 0)
				exit(1);
			garbage_collect(Fileid(t.get_val()));
			Fchar::unlock_stack();
		}
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
		t.getnext_nospc<Fchar>();
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
		if (DP())
			cout << "popd to " << dirstack.top().c_str() << endl;
		if (chdir(dirstack.top().c_str()) != 0)
			Error::error(E_FATAL, "popd: " + dirstack.top() + ": " + string(strerror(errno)));
		dirstack.pop();
	} else if (t.get_val() == "clear_include") {
		Pdtoken::clear_include();
		Pdtoken::clear_skipped();
	} else if (t.get_val() == "clear_defines")
		Pdtoken::macros_clear();
	else if (t.get_val() == "ro_prefix") {
		t.getnext_nospc<Fchar>();
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

	Metrics::call_metrics(&Metrics::add_ppdirective);
	t.getnext_nospc<Fchar>();
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
	else if (t.get_val() == "ident")	// GCC extension
		eat_to_eol();
	else
		/*
		 * @error
		 * An unkown preprocessor directive was found.
		 */
		Error::error(E_ERR, "Unknown preprocessor directive: " + t.get_val());
}


MCall *
Pdtoken::get_body_token_macro_mcall(Tokid t)
{
	mapMacroBody::const_iterator i;

	if (DP()) {
		cout << "Looking for " << t << " in " << macro_body_tokens.size() << " macro body elements\n";
		cout << "Map contents:\n";
		for (i = macro_body_tokens.begin(); i != macro_body_tokens.end(); i++)
		cout << i->first << "\n";
	}
	i = macro_body_tokens.find(t);
	if (i == macro_body_tokens.end())
		return NULL;
	else
		return i->second;
}

ostream&
operator<<(ostream& o,const dequePtoken &dp)
{
	dequePtoken::const_iterator i;

	for (i = dp.begin(); i != dp.end(); i++)
		o << *i;
	return (o);
}

ostream&
operator<<(ostream& o,const vectorPdtoken &dp)
{
	for (auto i = dp.begin(); i != dp.end(); i++)
		o << i->get_c_val();
	return (o);
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
