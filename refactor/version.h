/*
 * (C) Copyright 2003-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Access the RCS and global version identifiers
 *
 * Include synopsis: version.h
 *
 * $Id: version.h,v 1.7 2009/01/15 14:32:57 dds Exp $
 */

#ifndef VERSION_
#define VERSION_

#include <string>

using namespace std;

class Version {
private:
	static char revision[];		// Git identifier
	static char date[];		// Version release date
	static char compiled[];		// Compilation date
public:
	// Return the global revision number
	static const string get_revision() {
		return string(revision, 0, 6);
	}

	static const string get_date() {
		return date;
	}
};

#endif // VERSION_
