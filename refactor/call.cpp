/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: call.cpp,v 1.15 2007/08/12 07:22:01 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <iostream>
#include <list>
#include <set>

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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "fdep.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "eclass.h"
#include "sql.h"

// Function currently being parsed
Call *Call::current_fun = NULL;
//
// Nested function definitions
stack <Call *> Call::nesting;

// Set of all functions
Call::fun_map Call::all;

// The current function makes a call to f
void
Call::register_call(Call *f)
{
	register_call(f->get_tokid(), f);
}

// The current function makes a call to id
void
Call::register_call(const Id *id)
{
	register_call(id->get_token(), id);
}

// The current function (token t) makes a call to id
void
Call::register_call(const Token &t, const Id *id)
{
	register_call(t.constituents().begin()->get_tokid(), id->get_fcall());
}

// The current function (tokid t) makes a call to f
void
Call::register_call(Tokid t, Call *f)
{
	if (!current_fun)
		return;
	MCall *m = Pdtoken::get_body_token_macro_mcall(t);
	Call *caller;

	if (m)
		/*
		 * f appears in a macro body, so this is a call
		 * from the macro to the function.
		 */
		caller = m;
	else
		// Function to function call
		caller = current_fun;

	register_call(caller, f);
}

// From calls to
void
Call::register_call(Call *from, Call *to)
{
	from->add_call(to);
	to->add_caller(from);
	if (DP())
		cout << from->name << " calls " << to->name << "\n";
}

// ctor; never call it if the call for t already exists
Call::Call(const string &s, const Token &t) :
		name(s),
		token(t)
{
	if (DP())
		cout << "Construct new call for " << s << '\n';
	all.insert(fun_map::value_type(t.get_parts_begin()->get_tokid(), this));
}

// Return true if e appears in the eclasses comprising our name
bool
Call::contains(Eclass *e) const
{
	for (dequeTpart::const_iterator i = get_token().get_parts_begin(); i != get_token().get_parts_end(); i++) {
		int len = i->get_len();
		Tokid t = i->get_tokid();
		for (int pos = 0; pos < len;) {
			Eclass *e2 = t.get_ec();
			if (e2 == NULL) {
				if (DP())
					cout << "No eclass for tokid " << t << "\n";
				return false;
			}
			if (e == e2)
				return true;
			t += e2->get_len();
			pos += e2->get_len();
		}
	}
	return false;
}

void
Call::clear_visit_flags()
{
	for (const_fmap_iterator_type i = all.begin(); i != all.end(); i++)
		i->second->visited = false;
}

void
Call::clear_print_flags()
{
	for (const_fmap_iterator_type i = all.begin(); i != all.end(); i++)
		i->second->printed = false;
}

Call *
Call::get_call(const Token &t)
{
	pair <const_fmap_iterator_type, const_fmap_iterator_type> maybe(all.equal_range(t.get_parts_begin()->get_tokid()));
	const_fmap_iterator_type i;

	for (i = maybe.first; i != maybe.second; i++) {
		if (DP())
			cout << "Compare " << t << " with " << i->second->get_token() << "\n";
		if (t.equals(i->second->get_token())) {
			if (DP())
				cout << "Get call for " << t << " returns " << &(i->second) << "\n";
			return i->second;
		}
	}
	return NULL;
}

pair <Call::const_fmap_iterator_type, Call::const_fmap_iterator_type>
Call::get_calls(Tokid t)
{
	return all.equal_range(t);
}

// Mark the function's span
void
Call::mark_begin()
{
	if (!begin.is_valid())
		begin = Fchar::get_context();
}

// Mark the function's span and add it to the corresponding file
void
Call::mark_end()
{
	csassert(begin.is_valid());
	if (end.is_valid())
		return;
	end = Fchar::get_context();
	if (is_span_valid())
		end.get_tokid().get_fileid().add_function(this);
}

// Return true if the span represents a file region
bool
Call::is_span_valid() const
{
	return begin.is_valid() && end.is_valid() &&
	    begin.get_tokid().get_fileid() == end.get_tokid().get_fileid() &&
	    begin.get_tokid().get_streampos() < end.get_tokid().get_streampos();
}

void
Call::unset_current_fun()
{
	csassert(current_fun);
	current_fun->mark_end();
	nesting.pop();
	if (nesting.empty())
		current_fun = NULL;
	else
		current_fun = nesting.top();
}

#ifdef COMMERCIAL
void
Call::dumpSql(Sql *db, ostream &of)
{
	for (const_fmap_iterator_type i = fbegin(); i != fend(); i++) {
		Call *fun = i->second;
		Tokid t = fun->get_site();
		of << "INSERT INTO FUNCTIONS VALUES(" <<
		ptr_offset(fun) << ", '" <<
		fun->name << "', " <<
		db->boolval(fun->is_macro()) << ',' <<
		db->boolval(fun->is_defined()) << ',' <<
		db->boolval(fun->is_declared()) << ',' <<
		db->boolval(fun->is_file_scoped()) << ',' <<
		t.get_fileid().get_id() << ',' <<
		(unsigned)(t.get_streampos()) << ");\n";

		int start = 0;
		for (dequeTpart::const_iterator j = fun->get_token().get_parts_begin(); j != fun->get_token().get_parts_end(); j++) {
			Tokid t2 = j->get_tokid();
			int len = j->get_len() - start;
			int pos, ord;
			for (ord = pos = 0; pos < len; ord++) {
				Eclass *ec = t2.get_ec();
				of << "INSERT INTO FUNCTIONID VALUES(" <<
				ptr_offset(fun) << ',' <<
				ord << ',' <<
				ptr_offset(ec) << ");\n";
				pos += ec->get_len();
				t2 += ec->get_len();
			}
			start += j->get_len();
		}


		for (Call::const_fiterator_type dest = fun->call_begin(); dest != fun->call_end(); dest++)
			of << "INSERT INTO FCALLS VALUES(" <<
			ptr_offset(fun) << ',' <<
			ptr_offset(*dest) << ");\n";
	}
}
#endif /* COMMERCIAL */
