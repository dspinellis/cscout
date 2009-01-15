/*
 * (C) Copyright 2008-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Web directory browsing functions
 *
 * $Id: dirbrowse.h,v 1.4 2009/01/15 14:32:57 dds Exp $
 */

#ifndef DIRBROWSE_
#define DIRBROWSE_

#include <cstdio>

using namespace std;

class Fileid;

// Add a file to the directory tree for later browsing
void *dir_add_file(Fileid f);
// Display a directory's contents
void dir_page(FILE *of, void *p);
// Display on of a URL for browsing the project's top dir
void dir_top(FILE *of, const char *name);

#endif /* DIRBROWSE_ */
