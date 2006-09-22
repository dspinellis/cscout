/*
 * (C) Copyright 2006 Diomidis Spinellis.
 *
 * A pager for HTML output
 *
 * $Id: pager.h,v 1.1 2006/09/22 09:20:41 dds Exp $
 */

class Pager {
private:
	FILE *of;		// Output file
	int pagesize;		// Elements per page
	int current;		// Number of current element
	int skip;		// Elements to skip; -1 means show all
	string url;		// Query URL
public:
	Pager(FILE *f, int ps, const string &qurl);
	bool show_next();
	void end();
};
