/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: tokid.cpp,v 1.9 2001/08/21 18:29:45 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "eclass.h"

TE_map Tokid::tm;

mapTokidEclass tokid_map;		// Dummy; used for printing
mapTokidEclass TE_map::tm;

ostream&
operator<<(ostream& o,const Tokid t)
{
	o << t.fi.get_path() << "(" << t.offs << ")";
	return o;
}

ostream&
operator<<(ostream& o,const mapTokidEclass& t)
{
	mapTokidEclass::const_iterator i;

	for (i = Tokid::tm.tm.begin(); i != Tokid::tm.tm.end(); i++) {
		o << (*i).first << ":\n";
		o << *((*i).second) << "\n\n";
	}
	return o;
}

dequeTpart
Tokid::constituents(int l)
{
	Tokid t = *this;
	dequeTpart r;
	mapTokidEclass::const_iterator e = tm.tm.find(t);

	if (e == Tokid::tm.tm.end()) {
		// No EC defined, create a new one
		new Eclass(t, l);
		Tpart tp(t, l);
		r.push_back(tp);
		return (r);
	}
	// Make r be the Tparts of the ECs covering our tokid t
	for (;;) {
		int covered = (e->second)->get_len();
		Tpart tp(t, covered);
		r.push_back(tp);
		l -= covered;
		assert(l >= 0);
		if (l == 0)
			return (r);
		t += covered;
		e = tm.tm.find(t);
		assert(e != Tokid::tm.tm.end());
	}
}

ostream&
operator<<(ostream& o,const dequeTokid& dt)
{
	dequeTokid::const_iterator i;

	for (i = dt.begin(); i != dt.end(); i++) {
		o << *i;
		if (i + 1 != dt.end())
			o << ", ";
	}
	return (o);
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp
// cl -GX -DWIN32 -DUNIT_TEST tokid.cpp eclass.obj fileid.obj kernel32.lib

main()
{
	Tokid a(Fileid("tokid.cpp"), 10);
	Tokid b(Fileid("./tokid.cpp"), 15);

	cout << (b - a) << " should be 5\n";
	cout << a << " should be tokid.cpp(10)\n";
	a += 4;
	a++;
	cout << a << " should be tokid.cpp(15)\n";
	cout << (a == b) << " should be 1\n";
	b = b + 1;
	cout << (a == b) << " should be 0\n";

	// Tests for the map
	Tokid c(Fileid("./tokid.cpp"), 5);
	Tokid d(Fileid("tokmap.cpp"), 10);
	Tokid e(Fileid("tokmap.cpp"), 15);
	Eclass e1(5);
	Eclass e2(3);

	e1.add_tokid(a);
	e1.add_tokid(b);
	e1.add_tokid(d);
	e2.add_tokid(e);
	e2.add_tokid(c);
	cout << tokid_map;

	// Test for the constituent
	Tokid x(Fileid("main.cpp"), 20);

	dequeTpart dt = x.constituents(10);
	cout << "Initial dt: " << dt << "\n";
	cout << "Split EC: " << *x.get_ec()->split(2);
	dt = x.constituents(10);
	cout << "dt after split:" << dt << "\n";

	return (0);
}
#endif /* UNIT_TEST */
