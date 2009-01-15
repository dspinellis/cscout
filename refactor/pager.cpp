/*
 * (C) Copyright 2006-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A pager for HTML output
 *
 * $Id: pager.cpp,v 1.6 2009/01/15 14:32:57 dds Exp $
 */

#include <string>
#include <set>
#include <algorithm>		// max
#include <cstdio>		// FILE
#include <cstdlib>		// atoi

#include "swill.h"
#include "getopt.h"

#include "cpp.h"
#include "debug.h"
#include "pager.h"

Pager::Pager(FILE *f, int ps, const string &qurl, bool bmk) : of(f), pagesize(ps), current(0), url(qurl), bookmarkable(bmk)
{
	if (!swill_getargs("I(skip)", &skip))
		skip = 0;
}

bool
Pager::show_next()
{
	bool ret = (skip == -1) || ((current >= skip) && (current < skip + pagesize));
	current++;
	return (ret);
}

void
Pager::end()
{
	// Total number of elements
	int nelem = current;
	// Total number of pages
	int npages = nelem / pagesize + (nelem % pagesize ? 1 : 0);
	int thispage = skip / pagesize;

	switch (nelem) {
	case 0:
		fprintf(of, "<p>No elements found.<br />");
		break;
	case 1:
		fprintf(of, "<p>One element found.<br />");
		break;
	default:
		if (skip == -1)
			fprintf(of, "<p>Elements 1 to %d of %d.<br />", nelem, nelem);
		else
			fprintf(of, "<p>Elements %d to %d of %d.<br />",
				thispage * pagesize + 1,
				min(thispage * pagesize + pagesize, nelem),
				nelem);
		break;
	}
	if (nelem > pagesize) {
		fputs("Page: ", of);
		if (skip > 0)
			fprintf(of, "<a class='pagen prev' href=\"%s&skip=%d\">previous</a> ", url.c_str(), skip - pagesize);
		for (int i = 0; i < npages; i++)
			if (i == thispage && skip != -1)
				fprintf(of, "<span class='pagen this'>%d</span> ", i + 1);
			else
				fprintf(of, "<a class='pagen' href=\"%s&skip=%d\">%d</a> ", url.c_str(), i * pagesize, i + 1);
		if (skip != -1 && thispage + 1 < npages)
			fprintf(of, "<a class='pagen next' href=\"%s&skip=%d\">next</a> ", url.c_str(), skip + pagesize);
		if (skip != -1)
			fprintf(of, "<a class='pagen all' href=\"%s&skip=-1\">all</a>", url.c_str());
		fputs("<br clear='all'/>", of);
	}
	if (bookmarkable)
		fprintf(of, "You can bookmark <a href=\"%s\">this link</a> to save the respective query.", url.c_str());
	fputs("</p>\n", of);
}
