/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: macro.cpp,v 1.41 2006/07/30 15:13:56 dds Exp $
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
#include <cstdlib>		// strtol
#include <cctype>		// isspace

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
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
 * If get_more is true when tokens is exhausted read using pltoken::getnext
 * Leave in tokens the first token not gathered.
 * If want_space is true return spaces, otherwise discard them
 */
Ptoken
arg_token(PtokenSequence& tokens, bool get_more, bool want_space)
{
	if (want_space) {
		if (!tokens.empty()) {
			Ptoken r(tokens.front());
			tokens.pop_front();
			return (r);
		} else if (get_more) {
			Pltoken t;
			t.getnext<Fchar>();
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
			Pltoken t;
			do {
				t.getnext_nospc<Fchar>();
			} while (t.get_code() != EOF && t.is_space());
			return (t);
		} else
			return Ptoken(EOF, "");
	}
}

/*
 * Get the macro arguments specified in formal_args, initiallly by
 * removing them from tokens, then, if get_more is true, from pltoken<fchar>.getnext.
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
				(t.get_code() == terminate)))
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
	for (i = p + 1; i != v.end(); i++) {
		if ((*i).get_code() == CPP_CONCAT)
			return (false);
		if (!(*i).is_space())
			break;
	}

	return (true);
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
find_formal_argument(const mapArgval &args, Ptoken t)
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
Macro::Macro( const Ptoken& name, bool id, bool isfun) :
	name_token(name),
	is_function(isfun),
	is_defined(id)
{
	if (isfun) {
		mcall = dynamic_cast<MCall *>(Call::get_call(name));
		if (!mcall)
			mcall = new MCall(name, name.get_name());
		if (DP())
			cout << "Mcall is " << mcall << "\n";
	} else
		mcall = NULL;	// To void nasty surprises
}

static PtokenSequence subst(const dequePtoken& is, const mapArgval &args, HideSet hs, PtokenSequence os, bool skip_defined, const Macro *caller);
static PtokenSequence hsadd(const HideSet& hs, const PtokenSequence& ts);
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
macro_expand(const PtokenSequence& ts, bool get_more, bool skip_defined, const Macro *caller)
{
	if (ts.empty())
		return (ts);

	const Ptoken &head(ts.front());

	if (DP()) cout << "Expanding: " << ts << endl;
	PtokenSequence tail(ts);
	tail.pop_front();

	// Skip the arguments of the defined operator, if needed
	if (skip_defined && head.get_code() == IDENTIFIER && head.get_val() == "defined") {
		PtokenSequence da(gather_defined_operator(tail));
		da.push_front(head);
		PtokenSequence rest(macro_expand(tail, get_more, skip_defined, caller));
		da.splice(da.end(), rest);
		return (da);
	}

	const string name = head.get_val();
	mapMacro::const_iterator mi(Pdtoken::macros_find(name));
	if (Pdtoken::macro_is_defined(mi)) {
		const Macro& m = mi->second;
		if (!head.hideset_contains(m.get_name_token())) {
			if (DP()) cout << "replacing for " << name << " tokens " << tail << endl;
			Token::unify((*mi).second.name_token, head);
			PtokenSequence removed_spaces;
			if (!m.is_function) {
				// Object-like macro
				HideSet hs(head.get_hideset());
				hs.insert(m.get_name_token());
				PtokenSequence s(subst(m.value, mapArgval(), hs, PtokenSequence(), skip_defined, caller));
				s.splice(s.end(), tail);
				return (macro_expand(s, get_more, skip_defined, &m));
			} else if (fill_in(tail, get_more, removed_spaces) && tail.front().get_code() == '(') {
				// Application of a function-like macro
				mapArgval args;			// Map from formal name to value

				if (DP())
					cout << "Expanding " << m << " inside " << caller << "\n";
				if (caller && caller->is_function)
					// Macro to macro call
					Call::register_call(caller->get_mcall(), m.get_mcall());
				else
					// Function to macro call
					Call::register_call(m.get_mcall());
				tail.pop_front();
				Ptoken close;
				if (!gather_args(name, tail, m.formal_args, args, get_more, m.is_vararg, close))
					return (PtokenSequence());	// Attempt to bail-out on error
				HideSet hs;
				set_intersection(head.get_hideset().begin(), head.get_hideset().end(),
					close.get_hideset().begin(), close.get_hideset().end(),
					inserter(hs, hs.begin()));
				hs.insert(m.get_name_token());
				PtokenSequence s(subst(m.value, args, hs, PtokenSequence(), skip_defined, caller));
				s.splice(s.end(), tail);
				return (macro_expand(s, get_more, skip_defined, &m));
			} else {
				// Function-like macro name lacking a (
				if (DP()) cout << "splicing: [" << removed_spaces << ']' << endl;
				tail.splice(tail.begin(), removed_spaces);
			}
		} else {
			// Skip the head token if it is in the hideset
			if (DP()) cout << "Skipping (head is in HS)" << endl;
		}
	}
	PtokenSequence r(macro_expand(tail, get_more, skip_defined, caller));
	r.push_front(head);
	return (r);
}

/*
 * Substitute the arguments args appearing in the input sequence
 * Result is created in the output sequence and finally has the specified
 * hide set added to it, before getting returned.
 */
static PtokenSequence
subst(const dequePtoken& is, const mapArgval &args, HideSet hs, PtokenSequence os, bool skip_defined, const Macro *caller)
{
	if (DP())
		cout << "subst: is=" << is << " os=" << os << endl;
	if (is.empty())
		return (hsadd(hs, os));
	Ptoken head(is.front());
	dequePtoken tail(is);
	dequePtoken::iterator ti;
	tail.pop_front();
	mapArgval::const_iterator ai;
	switch (head.get_code()) {
	case '#':
		for (ti = tail.begin(); ti != tail.end(); ti++)
			if (!ti->is_space() && (ai = find_formal_argument(args, *ti)) != args.end()) {
				tail.erase(tail.begin(), ++ti);
				os.push_back(stringize(ai->second));
				return (subst(tail, args, hs, os, skip_defined, caller));
			}
		break;
	case CPP_CONCAT:
		for (ti = tail.begin(); ti != tail.end(); ti++)
			if (!ti->is_space())
				if ((ai = find_formal_argument(args, *ti)) != args.end()) {
					tail.erase(tail.begin(), ++ti);
					if (ai->second.size() == 0)	// Only if actuals can be empty
						return (subst(tail, args, hs, os, skip_defined, caller));
					else
						return (subst(tail, args, hs, glue(os, ai->second), skip_defined, caller));
				} else {
					PtokenSequence t(ti, ti);
					tail.erase(tail.begin(), ++ti);
					return (subst(tail, args, hs, glue(os, t), skip_defined, caller));
				}
		break;
	default:
		if ((ai = find_formal_argument(args, head)) == args.end())
			break;
		for (ti = tail.begin(); ti != tail.end(); ti++)
			if (!ti->is_space())
				break;
		if (ti != tail.end() && ti->get_code() == CPP_CONCAT) {
			// Paste but not expand LHS, RHS
			if (ai->second.size() == 0) {	// Only if actuals can be empty
				tail.erase(tail.begin(), ++ti);	// Erase including ##
				for (ti = tail.begin(); ti != tail.end(); ti++)
					if (!ti->is_space())
						if ((ai = find_formal_argument(args, *ti)) != args.end()) {
							tail.erase(tail.begin(), ++ti);	// Erase the ## RHS
							PtokenSequence actual(ai->second);
							os.splice(os.end(), actual);
							return (subst(tail, args, hs, os, skip_defined, caller));
						} else
							break;
				return (subst(tail, args, hs, os, skip_defined, caller));
			} else {
				tail.erase(tail.begin(), ti);	// Erase up to ##
				PtokenSequence actual(ai->second);
				os.splice(os.end(), actual);
				return (subst(tail, args, hs, os, skip_defined, caller));
			}
		}
		// Othewise expand head
		PtokenSequence expanded(macro_expand(ai->second, false, skip_defined, caller));
		os.splice(os.end(), expanded);
		return (subst(tail, args, hs, os, skip_defined, caller));
	}
	os.push_back(head);
	return (subst(tail, args, hs, os, skip_defined, caller));
}

// Return a new token sequence with hs added to the hide set of every element of ts
static PtokenSequence
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
	while (ls.back().is_space())
		ls.pop_back();
	while (rs.front().is_space())
		rs.pop_front();
	if (ls.empty() && rs.empty())
		return (ls);
	Tchar::clear();
	if (!ls.empty()) {
		Tchar::push_input(ls.back());
		ls.pop_back();
	}
	if (!rs.empty()) {
		Tchar::push_input(rs.front());
		rs.pop_front();
	}
	Tchar::rewind_input();
	for (;;) {
		Pltoken t;
		t.getnext<Tchar>();
		if (t.get_code() == EOF)
			break;
		if (DP()) cout << "Result: " << t ;
		ls.push_back(t);
	}
	ls.splice(ls.end(), rs);
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
		Pltoken t;
		for (;;) {
			t.getnext<Fchar>();
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
