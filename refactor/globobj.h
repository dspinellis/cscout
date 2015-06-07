/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * A globally defined/referenced object (variable or function)
 *
 *
 */

#ifndef GLOBOBJ_
#define GLOBOBJ_

#include <string>
#include <set>

using namespace std;

#include "fileid.h"
#include "token.h"
#include "type.h"
#include "tokid.h"

class GlobObj {
private:
	string name;
	set <Fileid> defined;	// Files where this is defined
	set <Fileid> used;	// Files where this is used

	/*
	 * A token (almost) uniquely identifying the call entity.
	 * (See fun_map comment for more details on the "almost".)
	 * Examples:
	 * Function's first declaration
	 * (could also be reference if implicitly declared)
	 * Macro's definition
	 */
	Token token;
	Type type;			// Object's type

	// See the comment for fun_map on why this must be a multimap
	typedef multimap <Tokid, GlobObj *> glob_map;
	static glob_map all;	// All global objects
public:
	// ctor; never call it if the call for t already exists
	GlobObj(const Token &t, Type typ, const string &s);

	Token get_token() const { return token; }
	Type get_type() const { return type; }

	const string &get_name() const { return name; }

	// Set the global def/ref dependencies for all files
	static void set_file_dependencies();
	// Get a identifier for a given Token
	static GlobObj *get_glob(const Token &t);

	// Add a file where this object is defined
	void add_def(Fileid f) { defined.insert(f); }
	// Add a file where this object is used
	void add_ref(Fileid f) { used.insert(f); }

	// Dump the data in SQL format
	static void dumpSql(Sql *db, ostream &of);
};

#endif // GLOBOBJ_
