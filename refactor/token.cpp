/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: token.cpp,v 1.4 2001/08/21 20:07:36 dds Exp $
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
#include "ytab.h"

ostream& 
operator<<(ostream& o,const Tpart &t)
{
	cout << t.ti << "[l=" << t.len << "]";
	return o;
}

ostream& 
operator<<(ostream& o,const Token &t)
{
	cout << "Token code:" << t.name() << "(" << t.code << ")\n";
	cout << "Parts:" << t.parts << "\n";
	return o;
}

dequeTpart
Token::constituents() const
{
	dequeTpart r;
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++) {
		dequeTpart c = (*i).get_tokid().constituents((*i).get_len());
		copy(c.begin(), c.end(), back_inserter(r));
	}
	return (r);
}

void
homogenize(const dequeTpart &a, const dequeTpart &b)
{
	dequeTpart::const_iterator ai = a.begin();
	dequeTpart::const_iterator bi = b.begin();
	Eclass *ae = (*ai).get_tokid().get_ec();
	Eclass *be = (*bi).get_tokid().get_ec();
	int alen, blen;

	while (ai != a.end() && bi != b.end()) {
		alen = ae->get_len();
		blen = be->get_len();
		// cout << "alen=" << alen << " blen=" << blen << "\n";
		if (blen < alen) {
			ae = ae->split(blen - 1);
			bi++;
			if (bi != b.end())
				be = (*bi).get_tokid().get_ec();
		} else if (alen < blen) {
			be = be->split(alen - 1);
			ai++;
			if (ai != a.end())
				ae = (*ai).get_tokid().get_ec();
		} else if (alen == blen) {
			ai++;
			if (ai != a.end())
				ae = (*ai).get_tokid().get_ec();
			if (bi != b.end())
				be = (*bi).get_tokid().get_ec();
			bi++;
		}
	}
}

void
unify(const Token &a, const Token &b)
{
	// Get the constituent Tokids; they may have grown more than the parts
	dequeTpart ac = a.constituents();
	dequeTpart bc = b.constituents();
	// Make the constituents of same length
	//cout << "Before homogenization: " << "\n" << "a=" << a << "\n" << "b=" << b << "\n";
	homogenize(ac, bc);
	// Get the constituents again; homogenizing may have changed them
	ac = a.constituents();
	bc = b.constituents();
	// cout << "After homogenization: " << "\n" << "a=" << ac << "\n" << "b=" << bc << "\n";
	// Now merge the corresponding ECs
	dequeTpart::const_iterator ai, bi;
	for (ai = ac.begin(), bi = bc.begin(); ai != ac.end(); ai++, bi++)
		merge((*ai).get_tokid().get_ec(), (*bi).get_tokid().get_ec());
	assert(bi == bc.end());
}

ostream&
operator<<(ostream& o,const dequeTpart& dt)
{
	dequeTpart::const_iterator i;

	for (i = dt.begin(); i != dt.end(); i++) {
		o << *i;
		if (i + 1 != dt.end())
			o << ", ";
	}
	return (o);
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp tokname.cpp
// cl -GX -DWIN32 -DUNIT_TEST token.cpp tokid.obj eclass.obj tokname.obj fileid.obj kernel32.lib

main()
{
	Token t(IDENTIFIER);

	cout << t;

	return (0);
}
#endif /* UNIT_TEST */
