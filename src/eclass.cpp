/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
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
 * For documentation read the corresponding .h file
 *
 */

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <set>
#include <deque>
#include <vector>
#include <algorithm>
#include <list>
#include <stack>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "parse.tab.h"
#include "ctoken.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "filedetails.h"
#include "eclass.h"
#include "call.h"

// Remove references to the equivalence class from the tokid map
// Should be called when we delete the ec for good
void
Eclass::remove_from_tokid_map()
{
	if (DP())
		cout << "Destructing " << *this << "\n";
	for (setTokid::const_iterator i = members.begin(); i != members.end(); i++)
		(*i).erase_ec(this);
}

// Merge the source EC into the dst EC.
void
merge_into(Eclass *dst, Eclass *src)
{
	csassert(src->len == dst->len);
	if (DP())
		cout << "merge onto dst=" << dst << *dst << " src=" << src << *src << "\n";
	for (setTokid::const_iterator i = src->members.begin(); i != src->members.end(); i++)
		dst->add_tokid(*i);
	dst->merge_attributes(src);
	delete src;
}

/*
 * Merge smaller-sized EC into larger sized.
 * If the two are equal-sized merge b into a.
 * Return the merged EC.
 */
Eclass *
merge(Eclass *a, Eclass *b)
{
	if (a == b)
		return a;

	// It is more efficient to append the little at the end of the large one
	if (a->members.size() >= b->members.size()) {
		merge_into(a, b);
		return a;
	} else {
		merge_into(b, a);
		return b;
	}
}

// Split an equivalence class starting at the (0-based) character position
// pos returning the new EC receiving the Tokids split off at the end.
Eclass *
Eclass::split(int pos)
{
	int oldchars = pos;		// Characters to retain in the old EC
	if (DP())
		cout << "Split " << this << " pos=" << pos << *this;
	csassert(oldchars < len);
	Eclass *e = new Eclass(len - oldchars);
	for (setTokid::const_iterator i = members.begin(); i != members.end(); i++)
		e->add_tokid(*i + oldchars);
	e->attr = attr;
	len = oldchars;
	if (DP()) {
		cout << "Split A: " << *e;
		cout << "Split B: " << *this;
	}
	return (e);
}

ostream&
operator<<(ostream& o,const Eclass& ec)
{
	setTokid::const_iterator i;

	for (i = ec.members.begin(); i != ec.members.end(); i++) {
		Tpart p(*i, ec.len);
		o << "\t" << p << "\n";
	}
	return o;
}

void
Eclass::add_tokid(Tokid t)
{
	members.insert(t);
	t.set_ec(this);
	if (t.get_readonly()) {
		if (DP())
			cout << "readonly " << *this << "\n";
		set_attribute(is_readonly);
	}
	if (!Pdtoken::skipping()) {
		set_attribute(Project::get_current_projid());
		if (DP())
			cout << "Set_attribute for " << t << " to " << Project::get_current_projid() << "\n";
	}
}

void
Eclass::remove_tokid(Tokid t)
{
	members.erase(t);
	t.erase_ec(this);
}

// Return a set of all files where the equivalence class appears
IFSet
Eclass::sorted_files()
{
	set <Fileid, fname_order> r;
	setTokid::const_iterator i;

	for (i = members.begin(); i != members.end(); i++)
		r.insert(i->get_fileid());
	return (r);
}

// Return a set of all functions where the equivalence class appears
set <Call *>
Eclass::functions()
{
	set <Call *> r;
	setTokid::const_iterator i;

	for (i = members.begin(); i != members.end(); i++) {
		FCallSet fc(Filedetails::get_functions(i->get_fileid()));
		for (FCallSet::const_iterator j = fc.begin(); j != fc.end(); j++)
			if ((*j)->is_span_valid() &&
			    *i >= (*j)->get_begin().get_tokid() &&
			    *i <= (*j)->get_end().get_tokid())
				r.insert(*j);
	}
	return (r);
}

// Return true if this equivalence class is unintentionally unused
bool
Eclass::is_unused()
{
	if (attr.get_attribute(is_declared_unused))
		return (false);		// Programmer knows it
	if (members.size() == 1)
		return (true);
	// More complex case: see if all the members come from unified identical files
	Tokid amember(*members.begin());
	if (Filedetails::get_identical_files(amember.get_fileid()).size() == members.size())
		return (true);
	return (false);
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c tokid.cpp fileid.cpp
// cl -GX -DWIN32 -DUNIT_TEST eclass.cpp tokid.obj fileid.obj kernel32.lib

main()
{
	Tokid a(Fileid("tokid.cpp"), 10);
	Tokid b(Fileid("./tokid.cpp"), 15);
	// Need pointer so that the delete in merge will work
	Eclass *e1 = new Eclass(5);

	e1->add_tokid(a);
	e1->add_tokid(b);
	cout << "e1:\n" << *e1;

	Eclass *e2 = new Eclass(5);
	Tokid c(Fileid("tokid.h"), 1);
	Tokid d(Fileid("./tokid.h"), 5);
	e2->add_tokid(c);
	e2->add_tokid(d);
	cout << "e2:\n" << *e2;

	Eclass *enew = merge(e1, e2);
	cout << "merged:\n" << *enew;

	Eclass *es = enew->split(3);
	cout << "split 0:\n" << *enew << "\n";
	cout << "split 3:\n" << *es << "\n";

	return (0);
}
#endif /* UNIT_TEST */
