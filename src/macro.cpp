/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
#include <algorithm>
#include <functional>		// ptr_fun
#include <cctype>		// isspace

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
#include "type.h"
#include "call.h"
#include "mcall.h"


/*
 * Return a macro argument token from tokens
 * Used by gather_args.
 * If get_more is true when tokens is exhausted read using pdtoken::getnext_noexpand
 * (see explanation on that method's comment for why we use pdtoken, rather than pltoken)
 * Leave in tokens the first token not gathered.
 * If want_space is true return spaces, otherwise discard them
 */
static Ptoken
arg_token(PtokenSequence& tokens, bool get_more, bool want_space)
{
	if (want_space) {
		if (!tokens.empty()) {
			Ptoken r(tokens.front());
			tokens.pop_front();
			return (r);
		} else if (get_more) {
			Pdtoken t;
			t.getnext_noexpand();
			return (t);
		} else
			return Ptoken(EOF, "");
	} else {
		while (!tokens.empty() && tokens.front().is_space())
			tokens.pop_front();
		if (!tokens.empty()) {
			Ptoken r(tokens.front());
			tokens.pop_front();
			return (r);
		} else if (get_more) {
			Pdtoken t;
			do {
				t.getnext_noexpand_nospc();
			} while (t.get_code() != EOF && t.is_space());
			return (t);
		} else
			return Ptoken(EOF, "");
	}
}

/*
 * Get the macro arguments specified in formal_args, initiallly by
 * removing them from tokens, then, if get_more is true, from pdtoken.getnext_noexpand.
 * The opening bracket has already been gathered.
 * Build the map from formal name to argument value args.
 * Return in close the closing bracket token (used for its hideset)
 * Return true if ok, false on error.
 */
static bool
gather_args(const string& name, PtokenSequence& tokens, const dequePtoken& formal_args, mapArgval& args, bool get_more, bool is_vararg, Ptoken &close)
{
	Ptoken t;
	dequePtoken::const_iterator i;
	for (i = formal_args.begin(); i != formal_args.end(); i++) {
		PtokenSequence& v = args[(*i).get_val()];
		char terminate;
		if (i + 1 == formal_args.end())
			terminate = ')';
		else if (is_vararg && i + 2 == formal_args.end())
			terminate = '.';	// Vararg last argument is optional; terminate with ) or ,
		else
			terminate = ',';
		int bracket = 0;
		// Get a single argument
		for (;;) {
			t = arg_token(tokens, get_more, true);
			if (bracket == 0 && (
				(terminate == '.' && (t.get_code() == ',' || t.get_code() == ')')) ||
				(terminate != '.' && t.get_code() == terminate)))
					break;
			switch (t.get_code()) {
			case '(':
				bracket++;
				break;
			case ')':
				bracket--;
				break;
			case EOF:
				/*
				 * @error
				 * The end of file was reached while
				 * gathering a macro's arguments
				 */
				Error::error(E_ERR, "macro [" + name + "]: EOF while reading function macro arguments");
				return (false);
			}
			v.push_back(t);
		}
		if (DP()) cout << "Gather args returns: " << v << "\n";
		// Check if varargs last optional argument was not supplied
		if (terminate == '.' && t.get_code() == ')') {
			i++;
			// Instantiate argument with an empty value list
			args[(*i).get_val()];
			break;
		}
		close = t;
	}
	if (formal_args.size() == 0) {
		t = arg_token(tokens, get_more, false);
		if (t.get_code() != ')') {
			/*
			 * @error
			 * The arguments to a function-like macro did
			 * not terminate with a closing bracket
			 */
			Error::error(E_ERR, "macro [" + name + "]: close bracket expected for function-like macro");
			return (false);
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
stringize(const PtokenSequence& ts)
{
	string res;
	PtokenSequence::const_iterator pi;
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
 * Remove from tokens and return the elements comprising the arguments to the defined
 * operator, * such as "defined X" or "defined(X)"
 * This is the rule when processing #if #elif expressions
 */
static PtokenSequence
gather_defined_operator(PtokenSequence& tokens)
{
	PtokenSequence r;

	// Skip leading space
	while (tokens.front().get_code() == SPACE) {
		r.push_back(tokens.front());
		tokens.pop_front();
	}
	switch (tokens.front().get_code()) {
	case IDENTIFIER:
		// defined X form
		r.push_back(tokens.front());
		tokens.pop_front();
		return (r);
	case '(':
		// defined (X) form
		r.push_back(tokens.front());
		tokens.pop_front();
		// Skip space
		while (tokens.front().get_code() == SPACE) {
			r.push_back(tokens.front());
			tokens.pop_front();
		}
		if (tokens.front().get_code() != IDENTIFIER)
			goto error;
		r.push_back(tokens.front());
		tokens.pop_front();
		// Skip space
		while (tokens.front().get_code() == SPACE) {
			r.push_back(tokens.front());
			tokens.pop_front();
		}
		if (tokens.front().get_code() != ')')
			goto error;
		r.push_back(tokens.front());
		tokens.pop_front();
		return (r);
	default:
	error:
		/*
		 * @error
		 * The preprocessor operator
		 * <code>defined<code> was not used as
		 * <code>defined(<code><em>identifier</em><code>)</code> or
		 * <code>defined<code> <em>identifier</em>.
		 */
		Error::error(E_ERR, "Invalid use of the defined preprocessor operator");
		return (r);
	}
}

// Return an arg iterator if token is a formal argument
// otherwise end()
static inline mapArgval::const_iterator
find_formal_argument(const mapArgval &args, const Ptoken &t)
{
	if (DP())
		cout << "find formal argument: " << t << "\n";
	if (t.get_code() != IDENTIFIER)
		return args.end();
	else
		return args.find(t.get_val());
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

// Update the map to include the macro's body refering to the macro
void
Macro::register_macro_body(mapMacroBody &map) const
{
	for (dequePtoken::const_iterator i = value.begin(); i != value.end(); i++)
		for (dequeTpart::const_iterator j = i->get_parts_begin(); j != i->get_parts_end(); j++)
			map[j->get_tokid()] = this->mcall;
}

// Constructor
Macro::Macro( const Ptoken& name, bool id, bool isfun, bool isimmutable) :
	name_token(name),
	is_function(isfun),
	is_immutable(isimmutable),
	is_vararg(false),
	is_defined(id)
{
	if (isfun) {
		Token uname(name.unique());	// Take care of identical files
		mcall = dynamic_cast<MCall *>(Call::get_call(name));
		if (!mcall)
			mcall = new MCall(name, name.get_name());
		if (DP())
			cout << "Mcall is " << mcall << "\n";
	} else
		mcall = NULL;	// To void nasty surprises
}

static PtokenSequence subst(const Macro &m, dequePtoken is, const mapArgval &args, HideSet hs, bool skip_defined, const Macro *caller);
static PtokenSequence inline hsadd(const HideSet& hs, const PtokenSequence& ts);
static PtokenSequence glue(PtokenSequence ls, PtokenSequence rs);
static bool fill_in(PtokenSequence &ts, bool get_more, PtokenSequence &removed);

/*
 * Expand a token sequence
 * If skip_defined is true then the defined() keyword is not processed
 * The caller is used for registering invocations from one macro to another
 * This is an implementation of Dave Prosser's algorithm, listed in
 * X3J11/86-196
 */
PtokenSequence
macro_expand(PtokenSequence ts, bool get_more, bool skip_defined, const Macro *caller)
{
	PtokenSequence r;	// Return value

	if (DP()) cout << "Expanding: " << ts << endl;
	while (!ts.empty()) {
		const Ptoken head(ts.front());
		ts.pop_front();

		if (head.get_code() != IDENTIFIER) {
			// Only attempt to expand identifiers (not e.g. string literals)
			r.push_back(head);
			continue;
		}

		if (skip_defined && head.get_code() == IDENTIFIER && head.get_val() == "defined") {
			// Skip the arguments of the defined operator, if needed
			PtokenSequence da(gather_defined_operator(ts));
			r.push_back(head);
			r.splice(r.end(), da);
			continue;
		}

		const string name = head.get_val();
		mapMacro::const_iterator mi(Pdtoken::macros_find(name));
		if (!Pdtoken::macro_is_defined(mi)) {
			// Nothing to do if the identifier is not a macro
			r.push_back(head);
			continue;
		}

		const Macro& m = mi->second;
		if (head.hideset_contains(m.get_name_token())) {
			// Skip the head token if it is in the hideset
			if (DP()) cout << "Skipping (head is in HS)" << endl;
			r.push_back(head);
			continue;
		}

		if (DP()) cout << "replacing for " << name << " tokens " << ts << endl;
		PtokenSequence removed_spaces;
		if (!m.is_function) {
			// Object-like macro
			Token::unify((*mi).second.name_token, head);
			HideSet hs(head.get_hideset());
			hs.insert(m.get_name_token());
			PtokenSequence s(subst(m, m.value, mapArgval(), hs, skip_defined, caller));
			ts.splice(ts.begin(), s);
			caller = &m;
		} else if (fill_in(ts, get_more, removed_spaces) && ts.front().get_code() == '(') {
			// Application of a function-like macro
			Token::unify((*mi).second.name_token, head);
			mapArgval args;			// Map from formal name to value

			if (DP())
				cout << "Expanding " << m << " inside " << caller << "\n";
			if (caller && caller->is_function)
				// Macro to macro call
				Call::register_call(caller->get_mcall(), m.get_mcall());
			else
				// Function to macro call
				Call::register_call(m.get_mcall());
			ts.pop_front();
			Ptoken close;
			if (!gather_args(name, ts, m.formal_args, args, get_more, m.is_vararg, close))
				continue;	// Attempt to bail-out on error
			HideSet hs;
			set_intersection(head.get_hideset().begin(), head.get_hideset().end(),
				close.get_hideset().begin(), close.get_hideset().end(),
				inserter(hs, hs.begin()));
			hs.insert(m.get_name_token());
			PtokenSequence s(subst(m, m.value, args, hs, skip_defined, caller));
			ts.splice(ts.begin(), s);
			caller = &m;
		} else {
			// Function-like macro name lacking a (
			if (DP()) cout << "splicing: [" << removed_spaces << ']' << endl;
			ts.splice(ts.begin(), removed_spaces);
			r.push_back(head);
		}
	}
	return (r);
}

/*
 * Return the position of the first non-space token in the range [pos, end)
 * Return end, if no such token is found
 */
static inline dequePtoken::iterator
find_nonspace(dequePtoken::iterator pos, dequePtoken::iterator end)
{
	for (; pos != end; pos++)
		if (!pos->is_space())
			return (pos);
	return (end);
}

/*
 * Substitute the arguments args appearing in the input sequence is
 * Result is created in the output sequence os and finally has the specified
 * hide set added to it, before getting returned.
 */
static PtokenSequence
subst(const Macro &m, dequePtoken is, const mapArgval &args, HideSet hs, bool skip_defined, const Macro *caller)
{
	PtokenSequence os;	// output sequence

	while (!is.empty()) {
		if (DP())
			cout << "subst: is=" << is << " os=" << os << endl;
		const Ptoken head(is.front());
		is.pop_front();		// is is now the tail
		dequePtoken::iterator ti, ti2;
		mapArgval::const_iterator ai;
		switch (head.get_code()) {
		case '#':		// Stringizing operator
			ti = find_nonspace(is.begin(), is.end());
			if (ti != is.end() && (ai = find_formal_argument(args, *ti)) != args.end()) {
				is.erase(is.begin(), ++ti);
				os.push_back(stringize(ai->second));
				continue;
			}
			break;
		case CPP_CONCAT:
			ti = find_nonspace(is.begin(), is.end());
			if (ti != is.end()) {
				if ((ai = find_formal_argument(args, *ti)) != args.end()) {
					is.erase(is.begin(), ++ti);
					if (ai->second.size() != 0)	// Only if actuals can be empty
						os = glue(os, ai->second);
				} else {
					PtokenSequence t(ti, ti + 1);
					is.erase(is.begin(), ++ti);
					os = glue(os, t);
				}
				continue;
			}
			break;
		default:
			ti = find_nonspace(is.begin(), is.end());
			if (ti != is.end() && ti->get_code() == CPP_CONCAT) {
				/*
				 * Implement the following gcc extension:
				 * "`##' before a rest argument that is empty
				 * discards the preceding "," token from
				 *  the macro definition.
				 * Otherwise, break to process a non-formal
				 * argument in the default way
				 */
				if (head.get_code() == ','
				    && m.get_is_vararg()) {
					ti2 = find_nonspace(ti + 1, is.end());
					if (ti2 != is.end() && (ai = find_formal_argument(args, *ti2)) != args.end() && ai->second.size() == 0) {
						// All conditions satisfied; discard elements:
						// <non-formal> <##> <empty-formal>
						is.erase(is.begin(), ++ti2);
						continue;
					}
				}
				if ((ai = find_formal_argument(args, head)) == args.end())
					break;	// Non-formal arguments don't deserve special treatment
				// Paste but not expand LHS, RHS
				if (ai->second.size() == 0) {	// Only if actuals can be empty
					is.erase(is.begin(), ++ti);	// Erase including ##
					ti = find_nonspace(is.begin(), is.end());
					if (ti != is.end() && (ai = find_formal_argument(args, *ti)) != args.end()) {
						is.erase(is.begin(), ++ti);	// Erase the ## RHS
						PtokenSequence actual(ai->second);
						os.splice(os.end(), actual);
					}
				} else {
					is.erase(is.begin(), ti);	// Erase up to ##
					PtokenSequence actual(ai->second);
					os.splice(os.end(), actual);
				}
				continue;
			}
			if ((ai = find_formal_argument(args, head)) == args.end())
				break;
			// Othewise expand head
			PtokenSequence expanded(macro_expand(ai->second, false, skip_defined, caller));
			os.splice(os.end(), expanded);
			continue;
		}
		os.push_back(head);
	}
	return (hsadd(hs, os));
}

// Return a new token sequence with hs added to the hide set of every element of ts
static inline PtokenSequence
hsadd(const HideSet& hs, const PtokenSequence& ts)
{
	PtokenSequence r;
	for (PtokenSequence::const_iterator i = ts.begin(); i != ts.end(); i++) {
		Ptoken t(*i);
		t.hideset_insert(hs.begin(), hs.end());
		r.push_back(t);
	}
	if (DP()) cout << "hsadd returns: " << r << endl;
	return (r);
}

// Paste last of left side with first of right side
static PtokenSequence
glue(PtokenSequence ls, PtokenSequence rs)
{
	if (ls.empty())
		return (rs);
	while (!ls.empty() && ls.back().is_space())
		ls.pop_back();
	while (!rs.empty() && rs.front().is_space())
		rs.pop_front();
	if (ls.empty() && rs.empty())
		return (ls);
	Tchar::clear();
	if (!ls.empty()) {
		if (DP()) cout << "glue LS: " << ls.back() << endl;
		Tchar::push_input(ls.back());
		ls.pop_back();
	}
	if (!rs.empty()) {
		if (DP()) cout << "glue RS: " << rs.front() << endl;
		Tchar::push_input(rs.front());
		rs.pop_front();
	}
	Tchar::rewind_input();
	for (;;) {
		Pltoken t;
		t.getnext<Tchar>();
		if (t.get_code() == EOF)
			break;
		if (DP()) cout << "glue result: " << t << endl;
		ls.push_back(t);
	}
	ls.splice(ls.end(), rs);
	if (DP()) cout << "glue returns: " << ls << endl;
	return (ls);
}

/*
 * Try to ensure that ts has at least one non-space token
 * Return true if this is the case
 * Return any discarded space tokens in removed
 */
static bool
fill_in(PtokenSequence &ts, bool get_more, PtokenSequence &removed)
{
	while (!ts.empty() && ts.front().is_space()) {
		removed.push_back(ts.front());
		ts.pop_front();
	}
	if (!ts.empty())
		return (true);
	if (get_more) {
		Pdtoken t;
		for (;;) {
			t.getnext_noexpand();
			if (t.get_code() == EOF)
				return (false);
			else if (t.is_space())
				removed.push_back(t);
			else
				break;
		}
		ts.push_back(t);
		return (true);
	}
	return (false);
}
