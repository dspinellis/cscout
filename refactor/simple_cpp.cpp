/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Simple preprocessor.  
 * Handles trigraphs, line splicing, comments, white space.
 *
 * $Id: simple_cpp.cpp,v 1.9 2001/09/03 09:16:29 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "debug.h"

main(int argc, char *argv[])
{
	Fchar::set_input(argv[1]);

	Debug::db_read();
	Fchar::push_input("defs.h");

	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		if (t.get_code() == STRING_LITERAL)
			cout << "\"";
		else if (t.get_code() == CHAR_LITERAL)
			cout << "'";

		cout << t.get_val();

		if (t.get_code() == STRING_LITERAL)
			cout << "\"";
		else if (t.get_code() == CHAR_LITERAL)
			cout << "'";
	}

	return (0);
}
