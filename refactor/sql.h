/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Portable SQL database abstraction
 *
 * $Id: sql.h,v 1.5 2007/08/25 05:29:29 dds Exp $
 */


#ifdef COMMERCIAL
class Sql {
	// Instance of current engine
	static Sql *instance;
public:
	virtual ~Sql() {}
	// Set the database to the specified engine
	// Return true if OK
	static bool setEngine(const char *dbengine);
	static Sql *getInterface() { return instance; }
	virtual string escape(string s);
	virtual char * escape(char c);
	virtual const char *ptrtype() {
		return sizeof(void *) > 32 ? "BIGINT" : "INTEGER";
	}
	virtual const char *booltype() { return "BOOLEAN"; }
	virtual const char *varchar() { return "CHARACTER VARYING"; }
	virtual const char *boolval(bool v);
};

class Mysql: public Sql {
public:
	const char *booltype() { return "bool"; }
	const char *varchar() { return "TEXT"; }
	const char *boolval(bool v);
};

class Hsqldb: public Sql {
public:
	const char *varchar() { return "VARCHAR"; }
	char * escape(char c);
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

#endif /* COMMERCIAL */
