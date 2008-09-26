/*
 * (C) Copyright 2006 Diomidis Spinellis.
 *
 * A pager for HTML output
 *
 * $Id: pager.h,v 1.2 2008/09/26 05:34:33 dds Exp $
 */

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
