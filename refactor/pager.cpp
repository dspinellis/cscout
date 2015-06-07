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
