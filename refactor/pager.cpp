/*
 * (C) Copyright 2006 Diomidis Spinellis.
 *
 * A pager for HTML output
 *
 * $Id: pager.cpp,v 1.1 2006/09/22 09:20:41 dds Exp $
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

Pager::Pager(FILE *f, int ps, const string &qurl) : of(f), pagesize(ps), current(0), url(qurl)
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
		fputs("Select page: ", of);
		if (skip > 0)
			fprintf(of, "<a href=\"%s&skip=%d\">previous</a> ", url.c_str(), skip - pagesize);
		for (int i = 0; i < npages; i++)
			if (i == thispage)
				fprintf(of, "%d ", i + 1);
			else
				fprintf(of, "<a href=\"%s&skip=%d\">%d</a> ", url.c_str(), i * pagesize, i + 1);
		if (thispage + 1 < npages)
			fprintf(of, "<a href=\"%s&skip=%d\">next</a> ", url.c_str(), skip + pagesize);
		fprintf(of, "<a href=\"%s&skip=-1\">all</a><br />", url.c_str());
	}
	fprintf(of, "You can bookmark <a href=\"%s\">this link</a> to save the respective query.</p>", url.c_str());
}
