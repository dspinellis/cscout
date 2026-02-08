/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Web directory browsing functions
 *
 */

#ifndef DIRBROWSE_
#define DIRBROWSE_

#include <cstdio>

using namespace std;

class Fileid;

// Add a file to the directory tree for later browsing
void *dir_add_file(Fileid f);
// Display a directory's contents
int dir_page(FILE *of, void *p);
// Display on of a URL for browsing the project's top dir
void dir_top(FILE *of, const char *name);

#endif /* DIRBROWSE_ */
