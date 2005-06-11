/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Portable SQL database abstraction
 *
 * $Id: sql.h,v 1.2 2005/06/11 16:29:43 dds Exp $
 */


#ifdef COMMERCIAL
class Sql {
public:
	static Sql *getInstance(const char *dbengine);
	virtual string escape(string s);
	virtual char * escape(char c);
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
