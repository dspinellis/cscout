#include <iostream>
#include <map>
#include <string>
#include <deque>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"

TM_map tokid_map;		// Dummy; used for printing
TE_map Tokid::tm;
TM_map TE_map::tm;

Tokid::Tokid(Fileid f, offset_t o)
	: fi(f), offs(o)
{
}

ostream&
operator<<(ostream& o,const Tokid t)
{
	o << t.fi.get_path() << "(" << t.offs << ")";
	return o;
}

ostream&
operator<<(ostream& o,const TM_map& t)
{
	TM_map::iterator i;

	for (i = Tokid::tm.tm.begin(); i != Tokid::tm.tm.end(); i++) {
		o << (*i).first << ":\n";
		o << *((*i).second) << "\n\n";
	}
	return o;
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp
// cl -GX -DWIN32 -DUNIT_TEST tokid.cpp eclass.obj fileid.obj kernel32.lib

main()
{
	Tokid a(Fileid("tokid.cpp"), 10);
	Tokid b(Fileid("./tokid.cpp"), 15);

	cout << a << " should be tokid.cpp(10)\n";
	a += 5;
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
	return (0);
}
#endif /* UNIT_TEST */
