/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * Error reporting
 *
 * Include synopsis:
 * #include <iostream>
 * #include <string>
 * #include "tokid.h"
 *
 */

#ifndef ERROR_
#define ERROR_

#include <string>

using namespace std;

enum e_error_level {
	E_WARN,
	E_ERR,
	E_INTERNAL,
	E_FATAL,
	E_DEBUG
};

class Error {
private:
	static int num_errors;
	static int num_warnings;
	static bool parsing;
public:
	static void set_parsing(bool v);
	static void error(enum e_error_level level, string msg, bool showloc = true);
	static void assertion_error(const char *fname, int line, const char *condition);
	static int get_num_errors() {return num_errors;}
	static int get_num_warnings() {return num_warnings;}
	static void clear() {num_errors = num_warnings = 0;}
};

#ifdef NDEBUG
#define csassert(x)
#else
#define csassert(x) do { if (!(x)) Error::assertion_error(__FILE__, __LINE__, #x); } while (0)
#endif

#endif // ERROR_
