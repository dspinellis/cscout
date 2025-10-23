/*
 * (C) Copyright 2001-2025 Diomidis Spinellis
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


#ifndef SQL_
#define SQL_

class Sql {
	// Instance of current engine
	static Sql *instance;
public:
	virtual ~Sql() {}
	// Set the database to the specified engine
	// Return true if OK
	static bool setEngine(string_view dbengine);
	static Sql *getInterface() { return instance; }
	virtual const char * begin_commands() { return ""; };
	virtual const char * end_commands() { return ""; };
	virtual string escape(string s);
	virtual const char * escape(char c);
	virtual const char *ptrtype() {
		return sizeof(void *) > 4 ? "BIGINT" : "INTEGER";
	}
	virtual const char *booltype() { return "BOOLEAN"; }
	virtual const char *varchar() { return "CHARACTER VARYING"; }
	virtual const char *boolval(bool v);
};

class Mysql: public Sql {
public:
	const char * begin_commands() {
		return "SET SESSION sql_mode=NO_BACKSLASH_ESCAPES;\n";
	};
	const char *booltype() { return "bool"; }
	const char *varchar() { return "TEXT"; }
	const char *boolval(bool v);
};

class Hsqldb: public Sql {
public:
	const char *varchar() { return "LONGVARCHAR"; }
	const char * escape(char c);
};

class Postgres: public Sql {
public:
};

class Sqlite: public Sql {
public:
	const char *begin_commands();
	const char * escape(char c);
};


// Return SQL equivalent for the logical value v
inline const char *
Sql::boolval(bool v)
{
	return v ? "true" : "false";
}

inline const char *
Mysql::boolval(bool v)
{
	return v ? "1" : "0";
}

#endif // SQL_
