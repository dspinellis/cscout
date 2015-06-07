/*
 * (C) Copyright 2003-2015 Diomidis Spinellis
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
 * Function call graph information
 *
 */

#ifndef FCALL_
#define FCALL_

#include "call.h"
#include "type.h"
#include "tokid.h"
#include "token.h"

/*
 * Function call information is always associated with Id objects
 * It is thus guaranteed to match the symbol table lookup operations
 */

// C function calling information
class FCall : public Call {
private:
	Tokid definition;		// Function's definition
	Type type;			// Function's type
	bool defined;			// True if the function has been defined
public:
	// Set the C function currently being parsed
	static void set_current_fun(const Type &t);
	static void set_current_fun(const Id *id);

	virtual Tokid get_definition() const { return definition; }
	virtual bool is_defined() const { return defined; }
	virtual bool is_declared() const { return true; }
	virtual bool is_file_scoped() const { return type.is_static(); }
	virtual bool is_cfun() const { return true; }
	virtual bool is_macro() const { return false; }
	virtual const string & entity_type_name() const {
		static string s("C function");
		return (s);
	}
	FCall(const Token& t, Type typ, const string &s);

	virtual ~FCall() {}
};

#endif // FCALL_
