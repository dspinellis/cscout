/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: eclass.cpp,v 1.32 2006/09/21 12:25:09 dds Exp $
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
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"

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

Eclass *
merge(Eclass *a, Eclass *b)
{
	Eclass *little, *large;
	if (a == b)
		return a;
	if (DP())
		cout << "merge a=" << a << *a << " b=" << b << *b << "\n";
	csassert(a->len == b->len);
	// It is more efficient to append the little at the end of the large one
	if (a->members.size() > b->members.size()) {
		large = a;
		little = b;
	} else {
		large = b;
		little = a;
	}

	for (setTokid::const_iterator i = little->members.begin(); i != little->members.end(); i++)
		large->add_tokid(*i);
	large->merge_attributes(little);
	delete little;
	return (large);
}

// Split an equivalence class after the (0-based) character position
// pos returning the new EC receiving the split Tokids
Eclass *
Eclass::split(int pos)
{
	int oldchars = pos + 1;		// Characters to retain in the old EC
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

// Return a sorted vector of all files used
IFSet
Eclass::sorted_files()
{
	set <Fileid, fname_order> r;
	setTokid::const_iterator i;
	int j;

	for (i = members.begin(), j = 0; i != members.end(); i++)
		r.insert(((*i).get_fileid()));
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
	if (amember.get_fileid().get_identical_files().size() == members.size())
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

	Eclass *es = enew->split(2);
	cout << "split 0:\n" << *enew << "\n";
	cout << "split 2:\n" << *es << "\n";

	return (0);
}
#endif /* UNIT_TEST */
