/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Error reporting
 *
 * Include synopsis:
 * #include <iostream>
 * #include <string>
 * #include "tokid.h"
 *
 * $Id: error.h,v 1.5 2004/07/23 06:55:38 dds Exp $
 */

#ifndef ERROR_
#define ERROR_

enum e_error_level {
	E_WARN,
	E_ERR,
	E_INTERNAL,
	E_FATAL
};

class Error {
private:
	static int num_errors;
	static int num_warnings;
public:
	static void error(enum e_error_level level, string msg, bool showloc = true);
	static int get_num_errors() {return num_errors;}
	static int get_num_warnings() {return num_warnings;}
	static void clear() {num_errors = num_warnings = 0;}
};

#endif // ERROR_
