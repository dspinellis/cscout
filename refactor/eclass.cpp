/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: eclass.cpp,v 1.8 2001/08/22 19:33:33 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"

Eclass *
merge(Eclass *a, Eclass *b)
{
	Eclass *small, *large;

	//if (a->len != b->len) cout << "merge a=" << *a << " b=" << *b << "\n";
	assert(a->len == b->len);
	// It is more efficient to append the small at the end of the large one
	if (a->members.size() > b->members.size()) {
		large = a;
		small = b;
	} else {
		large = b;
		small = a;
	}

	dequeTokid::const_iterator i;
	for (i = small->members.begin(); i != small->members.end(); i++)
		large->add_tokid(*i);
	delete small;
	return (large);
}

Eclass *
Eclass::split(int pos)
{
	int oldchars = pos + 1;		// Characters to retain in the old EC
	assert(oldchars < len);
	Eclass *e = new Eclass(len - oldchars);
	dequeTokid::const_iterator i;
	for (i = members.begin(); i != members.end(); i++)
		e->add_tokid(*i + oldchars);
	len = oldchars;
	return (e);
}

ostream&
operator<<(ostream& o,const Eclass& ec)
{
	dequeTokid::const_iterator i;

	for (i = ec.members.begin(); i != ec.members.end(); i++) {
		Tpart p(*i, ec.len);
		o << "\t" << p << "\n";
	}
	return o;
}

void
Eclass::add_tokid(Tokid t)
{
	members.push_front(t);
	t.set_ec(this);
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
