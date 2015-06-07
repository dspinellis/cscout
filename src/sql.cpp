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
 * Portable SQL database abstraction
 *
 */

#include <cstring>
#include <iostream>

#include "cpp.h"
#include "sql.h"

// An instance of the database interface
Sql *Sql::instance;

// Return SQL equivalent of character c
const char *
Sql::escape(char c)
{
	static char str[2];

	switch (c) {
	case '\'': return "''";
	default:
		str[0] = c;
		return str;
	}
}

const char *
Hsqldb::escape(char c)
{
	static char str[2];

	switch (c) {
	case '\'': return "''";
	case '\n': return "\\u0000a";
	case '\r': return "\\u0000d";
	default:
		str[0] = c;
		return str;
	}
}

string
Sql::escape(string s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += escape(*i);
	return r;
}

bool
Sql::setEngine(const char *dbengine)
{
	if (strcmp(dbengine, "mysql") == 0)
		instance = new Mysql();
	else if (strcmp(dbengine, "hsqldb") == 0)
		instance = new Hsqldb();
	else if (strcmp(dbengine, "postgres") == 0)
		instance = new Postgres();
	else {
		cerr << "Unknown database engine " << dbengine << "\n";
		cerr << "Supported database engine types are: mysql postgres hsqldb\n";
		return false;
	}
	return true;
}
