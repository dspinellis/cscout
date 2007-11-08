/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * Access the RCS and global version identifiers
 *
 * Include synopsis: version.h
 *
 * $Id: version.h,v 1.5 2007/11/08 09:24:28 dds Exp $
 */

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
