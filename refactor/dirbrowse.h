/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * Web directory browsing functions
 *
 * $Id: dirbrowse.h,v 1.3 2008/12/04 15:19:06 dds Exp $
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
