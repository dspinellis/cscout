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
