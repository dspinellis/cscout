/* 
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * Access the RCS and global version strings
 *
 * Include synopsis: version.h
 *
 * $Id: version.h,v 1.1 2003/06/03 08:40:52 dds Exp $
 */

class Version {
public:
	static char ident[];		// RCS Id for all source files
	static char version[];		// Global version Id
};
