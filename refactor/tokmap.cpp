/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Create a token map file
 *
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <list>

#include "cpp.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "tokmap.h"

TM_map Tokmap::tm;
Tokmap tokmap;		// Dummy; used for printing

ostream&
operator<<(ostream& o,const Tokmap& t)
{
	TM_map::iterator i;

	for (i = Tokmap::tm.begin(); i != Tokmap::tm.end(); i++) {
		o << (*i).first << ":\n";
		o << *((*i).second) << "\n\n";
	}
	return o;
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -DUNIT_TEST tokmap.cpp eclass.obj tokid.obj fileid.obj kernel32.lib

main()
{
	Tokid a("tokid.cpp", 10);
	Tokid b("./tokid.cpp", 15);
	Tokid c("./tokid.cpp", 5);
	Tokid d("tokmap.cpp", 10);
	Tokid e("tokmap.cpp", 15);
	Eclass e1(5);
	Eclass e2(3);

	e1.add_tokid(a);
	e1.add_tokid(b);
	e1.add_tokid(d);
	e2.add_tokid(e);
	e2.add_tokid(c);
	cout << tokmap;
	return (0);
}
#endif /* UNIT_TEST */
