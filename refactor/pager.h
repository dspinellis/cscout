/*
 * (C) Copyright 2006-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
