/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * File handling utilities
 *
 * $Id: fileutils.h,v 1.2 2008/12/04 15:19:06 dds Exp $
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
