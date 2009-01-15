/*
 * (C) Copyright 2008-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * File handling utilities
 *
 * $Id: fileutils.h,v 1.3 2009/01/15 14:32:57 dds Exp $
 */

#ifndef FILEUTILS_
#define FILEUTILS_

#include <string>
#include <fstream>

using namespace std;

bool cscout_input_file(const string &basename, ifstream &in, string &fname);
bool cscout_output_file(const string &basename, ofstream &out, string &fname);
int unlink(const string &name);

#endif // FILEUTILS_
