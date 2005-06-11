/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Portable SQL database abstraction
 *
 * $Id: sql.cpp,v 1.2 2005/06/11 16:29:43 dds Exp $
 */

#ifdef COMMERCIAL
#include <cstring>
#include <iostream>

#include "cpp.h"
#include "sql.h"

// Return SQL equivalent of character c
char *
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

char *
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

Sql *
Sql::getInstance(const char *dbengine)
{
	if (strcmp(dbengine, "mysql") == 0)
		return new Mysql();
	else if (strcmp(dbengine, "hsqldb") == 0)
		return new Hsqldb();
	else if (strcmp(dbengine, "postgres") == 0)
		return new Postgres();
	else {
		cerr << "Unknown database engine " << dbengine << "\n";
		cerr << "Supported database engine types are: mysql postgres hsqldb\n";
		return NULL;
	}
}

#endif /* COMMERCIAL */
