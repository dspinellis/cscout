/*
 * (C) Copyright 2006-2015 Diomidis Spinellis
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
 * Operating-system dependent functionality
 *
 */

#ifndef OS_
#define OS_

#include <string>

using namespace std;

// Return an opaque string uniquely identifying the filename passed
const char *get_uniq_fname_string(const char *pathname);
// Convert a (potential relative) file path into an absolute path
const char *get_full_path(const char *pathname);
// Return true if pathname is an absolute file path
bool is_absolute_filename(const string &pathname);

#endif // OS_
