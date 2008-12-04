/*
 * (C) Copyright 2006 Diomidis Spinellis.
 *
 * A pager for HTML output
 *
 * $Id: pager.h,v 1.3 2008/12/04 15:19:06 dds Exp $
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
