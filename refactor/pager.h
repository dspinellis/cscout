/*
 * (C) Copyright 2006-2015 Diomidis Spinellis
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
 * A pager for HTML output
 *
 */

#ifndef PAGER_
#define PAGER_

#include <string>
#include <cstdio>

using namespace std;

class Pager {
private:
	FILE *of;		// Output file
	int pagesize;		// Elements per page
	int current;		// Number of current element
	int skip;		// Elements to skip; -1 means show all
	string url;		// Query URL
	bool bookmarkable;	// True if result is bookmarkable
public:
	Pager(FILE *f, int ps, const string &qurl, bool bmk);
	bool show_next();
	void end();
};

#endif // PAGER_
