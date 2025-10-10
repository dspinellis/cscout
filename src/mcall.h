/*
 * (C) Copyright 2004-2015 Diomidis Spinellis
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
 * Function call graph information. Macros.
 *
 */

#ifndef MCALL_
#define MCALL_

#include "call.h"
#include "macro.h"

// Function-like macro calling information
class MCall : public Call {
private:
public:
	// Set the macro currently being processed
	static void set_current_fun(Macro &m);

	virtual bool is_defined() const { return true; }
	virtual bool is_declared() const { return false; }
	virtual bool is_file_scoped() const { return true; }
	virtual Tokid get_definition() const { return get_tokid(); }
	virtual bool is_cfun() const { return false; }
	virtual bool is_macro() const { return true; }
	virtual const string & entity_type_name() const;

	// ctor; never call it if the call for t already exists
	MCall(const Token& tok, const string &s) :
		Call(s, tok)
	{}

	virtual ~MCall() {}
};

#endif // MCALL_
