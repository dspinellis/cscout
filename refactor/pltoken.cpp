/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pltoken.cpp,v 1.4 2001/08/24 20:20:09 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "ytab.h"
#include "error.h"
#include "pltoken.h"

enum e_cpp_context Pltoken::context = cpp_normal;

#ifdef ndef
ostream& 
operator<<(ostream& o,const Pltoken &t)
{
	o << (Ptoken)t;
	return (o);
}
#endif


#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp tokname.cpp token.cpp ptoken.cpp fchar.cpp
// cl -GX -DWIN32 -DUNIT_TEST pltoken.cpp ptoken.obj token.obj tokid.obj eclass.obj tokname.obj fileid.obj fchar.obj kernel32.lib


main()
{
	Fchar::set_input("test/toktest.c");

	for (;;) {
		Pltoken t;

		t.template getnext<Fchar>();
		if (t.get_code() == EOF)
			break;
		cout << t;
	}

	return (0);
}
#endif /* UNIT_TEST */
