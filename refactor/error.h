/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Error reporting
 *
 * Include synopsis:
 * #include <iostream>
 * #include <string>
 * #include "tokid.h"
 *
 * $Id: error.h,v 1.9 2009/01/15 14:32:57 dds Exp $
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
