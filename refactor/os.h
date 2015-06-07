/*
 * (C) Copyright 2006-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Operating-system dependent functionality
 *
 */

#ifndef OS_
#define OS_

#include <string>

using namespace std;

// Return an opaque string uniquely identifying the filename passed
const char *get_uniq_fname_string(const char *pathname);
// Convert a (potential relative) file path into an absolute path
const char *get_full_path(const char *pathname);
// Return true if pathname is an absolute file path
bool is_absolute_filename(const string &pathname);

#endif // OS_
