/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: error.cpp,v 1.7 2002/09/07 09:47:15 dds Exp $
 */

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stack>
#include <deque>
#include <map>
#include <vector>

#include "fileid.h"
#include "cpp.h"
#include "tokid.h"
#include "fchar.h"
#include "error.h"

int Error::num_errors;
int Error::num_warnings;


void
Error::error(enum e_error_level level, string msg, bool showloc)
{
	if (showloc)
		cerr << Fchar::get_path() << "(" << Fchar::get_line_num() << "): ";
	switch (level) {
	case E_WARN: cerr << "warning: "; break;
	case E_ERR: cerr << "error: "; break;
	case E_INTERNAL: cerr << "internal error: "; break;
	case E_FATAL: cerr << "fatal error: "; break;
	}
	cerr << msg << "\n";
	switch (level) {
	case E_WARN: num_warnings++; break;
	case E_ERR: 
		num_errors++;
		if (num_errors > 100)
			Error::error(E_FATAL, "error count exceeds 100; exiting", false);
		break;
	case E_INTERNAL: num_errors++; break;	// Should have an assertion before
	case E_FATAL: exit(1);
	}
}

#ifdef UNIT_TEST

#include "token.h"
#include "ptoken.h"
#include "ytab.h"
#include "pltoken.h"

main()
{
	Fchar::set_input("test/toktest.c");

	Error::error(E_WARN, "Error at beginning of file");
	for (;;) {
		Pltoken t;

		t.template getnext<Fchar>();
		if (t.get_code() == EOF)
			break;
		cout << t;
	}
	Error::error(E_WARN, "Error at EOF");

	return (0);
}
#endif /* UNIT_TEST */
