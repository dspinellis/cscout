/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: pdtoken.cpp,v 1.1 2001/08/20 21:09:58 dds Exp $
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
#include "pdtoken.h"

bool Pdtoken::at_bol = true;
listPdtoken Pdtoken::expand;
mapMacro Pdtoken::macros;			// Defined macros

void
Pdtoken::getnext()
{
	Pltoken t;

expand_get:
	if (!expand.empty()) {
		*this = expand.front();
		expand.pop_front();
		return;
	}
again:
	t.template getnext<Fchar>();
	if (at_bol) {
		switch (t.get_code()) {
		case SPACE:
		case '\n':
			goto again;
		case '#':
			process_directive();
			goto again;
		default:
			at_bol = false;
		}
	}
	switch (t.get_code()) {
	case '\n':
		at_bol = true;
		//goto again; To remove newlines at this step
		*this = t;
		break;
	case IDENTIFIER:
		if (macros.find(t.get_val()) != macros.end()) {
			expand_macro(t);
			goto expand_get;
		}
		// FALLTRHOUGH
	default:
		*this = t;
		break;
	}
}

void
Pdtoken::expand_macro(Pltoken t)
{
}

void
Pdtoken::process_directive()
{
}


#ifdef UNIT_TEST

main()
{
	Fchar::set_input("test/toktest.c");

	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		cout << t;
	}

	return (0);
}
#endif /* UNIT_TEST */
