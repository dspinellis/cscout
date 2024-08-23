/*
 * (C) Copyright 2024 Diomidis Spinellis
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
 * A token constructed from a database dump.
 *
 */

#ifndef DBTOKEN_
#define DBTOKEN_

#include <iostream>
#include <map>
#include <string>
#include <deque>

using namespace std;

#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "parse.tab.h"

class Dbtoken : public Token {
public:
	// Efficient constructor
	Dbtoken() : Token(IDENTIFIER) {}

	// Add a token part
	void add_part(Tokid t, const string &s) {
		parts.push_back(Tpart(t, s.length()));
		val += s;
	}

	// Clear
	void clear() {
		parts.clear();
		val.clear();
	}

	// Read tokids and their equivalence classes from file named f
	static void read_eclasses(const char *f);

	// Read identifiers from file named f
	static void read_ids(const char *f);

	// Output tokids and their equivalence classes to file named f
	static void dump_eclasses(const char *f);

	// Dump identifiers to file named f
	static void dump_ids(const char *f);
};
#endif /* DBTOKEN_ */
