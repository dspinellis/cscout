#include <iostream>
#include <map>
#include <string>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"

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

#ifdef UNIT_TEST
// cl -GX -DWIN32 -DUNIT_TEST tokid.cpp fileid.obj kernel32.lib

main()
{
	Tokid a("tokid.cpp", 10);
	Tokid b("./tokid.cpp", 15);

	cout << a << " should be tokid.cpp(10)\n";
	a += 5;
	cout << a << " should be tokid.cpp(15)\n";
	cout << (a == b) << " should be 1\n";
	b = b + 1;
	cout << (a == b) << " should be 0\n";
	return (0);
}
#endif /* UNIT_TEST */
