/*
 * Create a token map file
 *
 * $Id: tokmap.cpp,v 1.3 2002/09/17 10:53:02 dds Exp $
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
