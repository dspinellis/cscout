/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Simple preprocessor.  
 * Handles trigraphs, line splicing, comments, white space.
 *
 * $Id: simple_cpp.cpp,v 1.1 2001/08/20 21:03:56 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <stack>
#include <iterator>
#include <fstream>
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

main(int argc, char *argv[])
{
	Fchar::set_input(argv[1]);

	for (;;) {
		Pltoken t;

		t.template getnext<Fchar>();
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
