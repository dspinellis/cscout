/* 
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * Access the RCS and global version identifiers
 *
 * Include synopsis: version.h
 *
 * $Id: version.h,v 1.2 2003/06/04 04:39:58 dds Exp $
 */

class Version {
public:
	static char revision[];		// Global revision number 
	static char date[];		// Version release date
	static char *ident[];		// RCS Id for all source files
};
