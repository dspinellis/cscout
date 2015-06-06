/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Portable SQL database abstraction
 *
 * $Id: sql.h,v 1.11 2015/06/06 23:41:28 dds Exp $
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
	static bool setEngine(const char *dbengine);
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
	const char *varchar() { return "VARCHAR"; }
	const char * escape(char c);
};

class Postgres: public Sql {
public:
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
