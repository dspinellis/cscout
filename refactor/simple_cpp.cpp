/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Simple preprocessor.
 * Handles trigraphs, line splicing, comments, white space.
 *
 * $Id: simple_cpp.cpp,v 1.17 2006/06/18 19:34:46 dds Exp $
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

#include "cpp.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "debug.h"

main(int argc, char *argv[])
{
	Fchar::set_input(argv[1]);

	Debug::db_read();
	//Fchar::push_input("/home/dds/src/refactor/defs.h");

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
