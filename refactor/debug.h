/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Debugpoints can be set for selectively enabling debugging output
 * The debugpoint() macro should be used to efficiently disable debugging output
 * A debugpoint on a file's 0 line will enable all debugpoints for that file
 *
 */

#ifndef DEBUG_
#define DEBUG_

#include <string>
#include <set>

using namespace std;

class Debug {
private:
	int k;
	string fname;
	int line;

	static set<Debug> dp;		// Enabled debug points
	static bool enabled;		// Global enable variable
public:
	Debug(const string f, int l) : fname(f), line(l) {};

	static inline bool is_db_set(const string &fname, int line);
	static void db_set(string fname, int line);
	static void db_read();
	static inline bool is_enabled() { return enabled; };
	inline friend bool operator <(const class Debug a, const class Debug b);
};

#ifdef NO_DP
#define DP() 0
#else
#define DP() (Debug::is_enabled() && Debug::is_db_set((const string)__FILE__, __LINE__))
#endif

inline bool
operator <(const class Debug a, const class Debug b)
{
	if (a.fname == b.fname)
		return (a.line < b.line);
	else
		return (a.fname < b.fname);
}

inline bool
Debug::is_db_set(const string &fname, int line)
{
	return (dp.find(Debug(fname, line)) != dp.end()) ||
	       (dp.find(Debug(fname, 0)) != dp.end());
}

#endif /* DEBUG_ */
