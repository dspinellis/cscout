/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
