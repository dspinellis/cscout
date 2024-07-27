/*
 * (C) Copyright 2003-2024 Diomidis Spinellis
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
#include "parse.tab.h"
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
#include "workdb.h"

// Function currently being parsed
Call *Call::current_fun = NULL;
//
// Nested function definitions
stack <Call *> Call::nesting;

// Set of all functions
Call::fun_map Call::all;

// Nested statements created from macro expansion
int Call::macro_nesting;

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
		pre_cpp_metrics(this),
		post_cpp_metrics(this),
		curr_stmt_nesting(0),
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
populate_namers()
{
	for (Call::const_fmap_iterator_type i = Call::fbegin(); i != Call::fend(); i++) {
	}
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
		Filedetails::add_function(end.get_tokid().get_fileid(), this);
	pre_cpp_metrics.summarize_operators();
	pre_cpp_metrics.done_processing();
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
	auto& metrics = current_fun->post_cpp_metrics;
	if (!metrics.is_processed()) {
		metrics.summarize_operators();
		metrics.summarize_identifiers();
		metrics.done_processing();
	}
	if (nesting.empty())
		current_fun = NULL;
	else
		current_fun = nesting.top();
}

// See if we have started nesting through macro-expanded tokens
void
Call::check_macro_nesting(const Ctoken &t)
{
	if (current_fun && !current_fun->pre_cpp_metrics.is_processed() &&
	    t.has_ec_attribute(is_macro_token))
		macro_nesting++;
}

void
Call::dumpSql(Sql *db, ostream &of)
{
	// First define all functions
	for (const_fmap_iterator_type i = fbegin(); i != fend(); i++) {
		Call *fun = i->second;
		Tokid t = fun->get_site();
		if (table_is_enabled(t_functions))
			of << "INSERT INTO FUNCTIONS VALUES("
			    << ptr_offset(fun) << ", '"
			    << fun->name << "', "
			    << db->boolval(fun->is_macro()) << ','
			    << db->boolval(fun->is_defined()) << ','
			    << db->boolval(fun->is_declared()) << ','
			    << db->boolval(fun->is_file_scoped()) << ','
			    << t.get_fileid().get_id() << ','
			    << (unsigned)(t.get_streampos()) << ','
			    << fun->get_num_caller()
			    << ");\n";

		if (fun->is_defined() && table_is_enabled(t_functiondefs)) {
			of << "INSERT INTO FUNCTIONDEFS VALUES(" << ptr_offset(fun)
			    << ',' << fun->get_begin().get_tokid().get_fileid().get_id() <<
			    ',' << (unsigned)(fun->get_begin().get_tokid().get_streampos()) <<
			    ',' << fun->get_end().get_tokid().get_fileid().get_id() <<
			    ',' << (unsigned)(fun->get_end().get_tokid().get_streampos());
			of << ");\n";
		}
		if (fun->is_defined() && table_is_enabled(t_functionmetrics)) {
			of << "INSERT INTO FUNCTIONMETRICS VALUES("
			    << ptr_offset(fun) << ',' << db->boolval(true);
			for (int j = 0; j < FunMetrics::metric_max; j++) {
				if (Metrics::is_internal<FunMetrics>(j))
					continue;
				if (Metrics::is_pre_cpp<FunMetrics>(j))
					of << ',' << fun->get_pre_cpp_metrics().get_metric(j);
				else
					cout << ",NULL";
			}
			of << ");\n";
			of << "INSERT INTO FUNCTIONMETRICS VALUES("
			    << ptr_offset(fun) << ',' << db->boolval(false);
			for (int j = 0; j < FunMetrics::metric_max; j++) {
				if (Metrics::is_internal<FunMetrics>(j))
					continue;
				if (Metrics::is_post_cpp<FunMetrics>(j))
					of << ',' << fun->get_post_cpp_metrics().get_metric(j);
				else
					cout << ",NULL";
			}
			of << ");\n";
		}

		int start = 0, ord = 0;
		for (dequeTpart::const_iterator j = fun->get_token().get_parts_begin(); j != fun->get_token().get_parts_end(); j++) {
			Tokid t2 = j->get_tokid();
			int len = j->get_len() - start;
			int pos = 0;
			while (pos < len) {
				Eclass *ec = t2.get_ec();
				if (table_is_enabled(t_functionid))
					of << "INSERT INTO FUNCTIONID VALUES("
					    << ptr_offset(fun) << ','
					    << ord << ','
					    << ptr_offset(ec) << ");\n";
				pos += ec->get_len();
				t2 += ec->get_len();
				ord++;
			}
			start += j->get_len();
		}
	}

	// Then their calls to satisfy integrity constraints
	if (table_is_enabled(t_fcalls))
		for (const_fmap_iterator_type i = fbegin(); i != fend(); i++) {
			Call *fun = i->second;
			for (Call::const_fiterator_type dest = fun->call_begin(); dest != fun->call_end(); dest++)
				of << "INSERT INTO FCALLS VALUES(" <<
				    ptr_offset(fun) << ',' <<
				    ptr_offset(*dest) << ");\n";
		}
}
