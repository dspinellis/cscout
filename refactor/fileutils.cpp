/*
 * (C) Copyright 2008-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * File handling utilities
 *
 */

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <cstdlib>		// getenv

#if defined(unix) || defined(__MACH__)
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#include <unistd.h>		// unlink
#elif defined(WIN32)
#include <io.h>			// mkdir
#include <fcntl.h>		// O_BINARY
#endif

using namespace std;

#include "debug.h"

/*
 * Return directories where CScout should be storing configuration files
 * The path includes four different directories:
 * .cscout, $CSCOUT_HOME, $HOME/.cscout, and $APPDATA/.cscout
 */
static vector <string>
cscout_dirs()
{
	vector <string> dirs;
#if !defined(PRODUCTION)
	dirs.push_back("../refactor");
#endif
	dirs.push_back(".cscout");
	if (getenv("CSCOUT_HOME"))
		dirs.push_back(getenv("CSCOUT_HOME"));
	if (getenv("HOME"))
		dirs.push_back(string(getenv("HOME")) + "/.cscout");
	if (getenv("APPDATA"))
		dirs.push_back(string(getenv("HOME")) + "/.cscout");
	return dirs;
}

/*
 * Attempt to open a CScout file for input, setting in to the file stream
 * and fname to the file name used.
 * Return true on success, false on error.
 */
bool
cscout_input_file(const string &basename, ifstream &in, string &fname)
{
	vector <string> dirs = cscout_dirs();
	vector <string>::const_iterator i;

	for (i = dirs.begin(); i != dirs.end(); i++) {
		fname = *i + "/" + basename;
		in.open(fname.c_str());
		if (in.fail())
			in.clear();
		else {
			if (DP())
				cout << "Input file is " << fname << endl;
			return true;
		}
	}
	return false;
}


/*
 * Attempt to open a CScout file for output, setting out to the file stream
 * and fname to the file name used.
 * Return true on success, false on error.
 */
bool
cscout_output_file(const string &basename, ofstream &out, string &fname)
{
	vector <string> dirs = cscout_dirs();
	vector <string>::const_iterator i;

	// First try to see if an existing .cscout directory exists
	for (i = dirs.begin(); i != dirs.end(); i++) {
		fname = *i + "/" + basename;
		out.open(fname.c_str());
		if (out.fail())
			out.clear();
		else
			return true;
	}
	// Then try to create it
	for (i = dirs.begin(); i != dirs.end(); i++) {
		#if defined(unix) || defined(__MACH__)
		(void)mkdir(i->c_str(), 0777);
		#else
		(void)mkdir(i->c_str());
		#endif
		fname = *i + "/" + basename;
		out.open(fname.c_str());
		if (out.fail())
			out.clear();
		else
			return true;
	}
	return false;
}

int
unlink(const string &name)
{
	return unlink(name.c_str());
}
