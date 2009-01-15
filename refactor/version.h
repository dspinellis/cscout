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
	static char revision[];		// RCS Revision tag
	static char date[];		// Version release date
public:
	// Return the global revision number
	static const string get_revision() {
		// 1.5 $
		string tail(revision + 11);
		// 1.5
		string head(tail, 0, tail.length() - 2);
		return head;
	}

	static const string get_date() {
		// 2003/... $
		string tail(date + 7);
		// Remove trailing " $"
		string head(tail, 0, tail.length() - 2);
		return head;
	}

	// Think of an accessor when we need it
	static const char *ident[];		// RCS Id for all source files
};

#endif // VERSION_
