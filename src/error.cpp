/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * For documentation read the corresponding .h file
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <deque>
#include <sstream>		// ostringstream
#include <map>
#include <vector>
#include <list>
#include <set>

#include "cpp.h"
#include "error.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"
#include "pdtoken.h"

int Error::num_errors;
int Error::num_warnings;
bool Error::parsing;


void
Error::error(enum e_error_level level, string msg, bool showloc)
{
	// Show all debugging output up to this point
	if (Debug::is_enabled())
		cout << flush;
	if (showloc)
		cerr << Fchar::get_path() << ':' << Fchar::get_line_num() << ": ";
	string name;
	switch (level) {
	case E_DEBUG: name = "debug message"; break;
	case E_WARN: name = "warning"; break;
	case E_ERR: name = "error"; break;
	case E_INTERNAL: name = "internal error"; break;
	case E_FATAL: name = "fatal error"; break;
	}
	cerr << name << ": " << msg << endl;
	switch (level) {
	case E_DEBUG: break;
	case E_WARN: num_warnings++; break;
	case E_ERR:
		num_errors++;
		if (num_errors > 100
				&& num_errors > Fchar::get_total_lines() / 10)
			/*
			 * @error
			 * To avoid cascading errors, the processing terminates
			 * when errors exceed 100 and 10% of processed lines
			 */
			Error::error(E_FATAL, "error count exceeds 100 and 10%; exiting", false);
		break;
	case E_INTERNAL: num_errors++; break;	// Should have an assertion before
	case E_FATAL: exit(1);
	}

	const vectorPdtoken& line = Pdtoken::get_current_line();
	if (showloc && line.size() > 0 && line[0].get_code() != EOF)
		cerr << "The text leading to this " << name << " is: [" <<
			Pdtoken::get_current_line() << "]\n";
}

void
Error::set_parsing(bool v)
{
	parsing = v;
}

void
Error::assertion_error(const char *fname, int line, const char *condition)
{
	ostringstream msg;
	msg << "assertion error: location: " << fname << ':' << line << " condition: \"" << condition << '"';
	error(E_INTERNAL, msg.str(), parsing);
}

#ifdef UNIT_TEST

#include "token.h"
#include "ptoken.h"
#include "parse.tab.h"
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
