#include <map>
#include <string>

#include "fileid.h"

int Fileid::counter;		// To generate ids
FI_uname_to_id Fileid::u2i;	// From unique name to id
FI_id_to_path Fileid::i2p;	// From id to full path
Fileid Fileid::anonymous = Fileid("ANONYMOUS", 0);

#ifdef WIN32
#include <windows.h>

static char *
get_uniq_fname_string(const char *name)
{
	static char buff[4096];
	LPTSTR nptr;

	// XXX Report an error if return value is not 0
	GetFullPathName(name, sizeof(buff), buff, &nptr);
	return (buff);
}
#endif /* WIN32 */

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>

static char *
get_uniq_fname_string(const char *name)
{
	static char buff[4096];
	struct stat sb;

	// XXX Report an error if return value is not 0
	stat(name, &sb);
	sprintf(buff, "%s:%s", sb._st_dev, sb.st_ino);
}
#endif /* unix */

Fileid::Fileid(const string &name)
{
	string s(get_uniq_fname_string(name.c_str()));
	FI_uname_to_id::iterator i;

	if ((i = u2i.find(s)) == u2i.end()) {
		// New filename; add a new fname/id pair in the map table
		u2i[s] = counter;
		// XXX Set to full path (using getwd under Unix)
		i2p[counter] = name;
		id = counter++;
	} else
		// Filename exists; our id is the one from the map
		id = (*i).second;
}

Fileid::Fileid(const string &name, int i)
{
	u2i[name] = i;
	i2p[i] = name;
	id = i;
	counter = i + 1;
}

string
Fileid::get_path() const
{
	return i2p[id];
}


#ifdef UNIT_TEST
// cl -GX -DWIN32 -DUNIT_TEST fileid.cpp kernel32.lib

#include <iostream>

main()
{
	Fileid x1;
	Fileid x2;
	Fileid a("fileid.cpp");
	Fileid b("./fileid.cpp");
	Fileid c(".");
	Fileid d = b;
	Fileid e(c);

	cout << "a=" << a.get_path() << " (should be fileId.cpp)\n";
	cout << "b=" << b.get_path() << " (should be fileId.cpp)\n";
	cout << "c=" << c.get_path() << " (should be .)\n";
	cout << "d=" << d.get_path() << " (should be fileId.cpp)\n";
	cout << "e=" << e.get_path() << " (should be .)\n";
	cout << "a==b: " << (a == b) << " (should be 1)\n";
	cout << "size=" << sizeof(a) << " (it better be 4)\n";
	cout << "x2=" << x2.get_path() << " (should be ANONYMOUS)\n";
	cout << "x1==x2: " << (x1 == x2) << " (should be 1)\n";
	return (0);
}
#endif
