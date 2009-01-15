/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: tokid.cpp,v 1.30 2009/01/15 14:32:57 dds Exp $
 */

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <deque>
#include <set>
#include <vector>
#include <stack>
#include <list>

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


mapTokidEclass Tokid::tm;		// Map from tokens to their equivalence

mapTokidEclass tokid_map;		// Dummy; used for printing

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

	for (i = Tokid::tm.begin(); i != Tokid::tm.end(); i++) {
		// Convert Tokids into Tparts to also display their content
		Tokid t = (*i).first;
		Eclass e = *((*i).second);
		Tpart p(t, e.get_len());
		o << p << ":\n";
		o << e << "\n\n";
	}
	return o;
}

// Clear the map of tokid equivalence classes
void
Tokid::clear()
{
	mapTokidEclass::const_iterator i;
	set <Eclass *> es;

	if (DP()) cout << "Have " << Tokid::tm.size() << " tokids\n";
	// First create a set of all ecs
	for (i = Tokid::tm.begin(); i != Tokid::tm.end(); i++)
		es.insert((*i).second);
	// Then free them
	if (DP()) cout << "Deleting " << es.size() << " classes\n";
	set <Eclass *>::const_iterator si;
	for (si = es.begin(); si != es.end(); si++) {
		if (DP()) cout << "Delete class " << *si << "\n";
		delete (*si);
	}
	// Finally, clear the map
	tm.clear();
}

dequeTpart
Tokid::constituents(int l)
{
	Tokid t = *this;
	dequeTpart r;
	mapTokidEclass::const_iterator e = tm.find(t);

	if (e == Tokid::tm.end()) {
		// No EC defined, create a new one
		new Eclass(t, l);
		Tpart tp(t, l);
		r.push_back(tp);
		return (r);
	}
	// Make r be the Tparts of the ECs covering our tokid t
	for (;;) {
		if (DP())
			cout << "Tokid = " << (e->first) << " Eclass = " << e->second << "\n" << (*(e->second)) << "\n";
		int covered = (e->second)->get_len();
		if (!Pdtoken::skipping()) {
			// Add the existing classes to our current project
			(e->second)->set_attribute(Project::get_current_projid());
			if (DP())
				cout << "Set projid to " << Project::get_current_projid() << "\n";
		}
		Tpart tp(t, covered);
		r.push_back(tp);
		if (DP())
			cout << "l = " << l << "covered = " << covered << endl;
		l -= covered;
		csassert(l >= 0);
		if (l == 0)
			return (r);
		t += covered;
		e = tm.find(t);
		// csassert(e != Tokid::tm.end());
		// Can only happen if we are deleting ECs with -m
		if (e == Tokid::tm.end()) {
			// No EC defined, create a new one covering the rest
			new Eclass(t, l);
			Tpart tp(t, l);
			r.push_back(tp);
			return (r);
		}
	}
}

// Set the Tokid's equivalence class attribute
void
Tokid::set_ec_attribute(enum e_attribute a, int l) const
{
	Tokid t = *this;
	dequeTpart r;
	mapTokidEclass::const_iterator e = tm.find(t);

	if (e == Tokid::tm.end()) {
		// No EC defined, create a new one
		Eclass *e = new Eclass(t, l);
		e->set_attribute(a);
		return;
	}
	// Set the ECs covering our tokid t
	for (;;) {
		int covered = (e->second)->get_len();
		(e->second)->set_attribute(a);
		l -= covered;
		csassert(l >= 0);
		if (l == 0)
			return;
		t += covered;
		e = tm.find(t);
		csassert(e != Tokid::tm.end());
	}
}

// Return true if one of the tokid's ECs has the specified attribute
bool
Tokid::has_ec_attribute(enum e_attribute a, int l) const
{
	Tokid t = *this;
	dequeTpart r;
	mapTokidEclass::const_iterator e = tm.find(t);

	if (e == Tokid::tm.end())
		// No EC defined, create a new one
		return false;
	// Check the ECs covering our tokid t
	for (;;) {
		int covered = (e->second)->get_len();
		if ((e->second)->get_attribute(a))
			return true;
		l -= covered;
		csassert(l >= 0);
		if (l == 0)
			return false;
		t += covered;
		e = tm.find(t);
		csassert(e != Tokid::tm.end());
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

Tokid
Tokid::unique() const
{
	if (DP()) {
		cout << "unique input:  " << *this << endl;
		cout << "unique output: " << Tokid(*(fi.get_identical_files().begin()), offs) << endl;
	}
	return Tokid(*(fi.get_identical_files().begin()), offs);
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
