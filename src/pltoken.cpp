/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <vector>
#include <set>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "parse.tab.h"
#include "token.h"
#include "ctoken.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"

enum e_cpp_context Pltoken::context = cpp_normal;
bool Pltoken::semicolon_line_comments;
bool Pltoken::echo;

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
