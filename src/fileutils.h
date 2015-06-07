/*
 * (C) Copyright 2008-2015 Diomidis Spinellis
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
 * File handling utilities
 *
 */

#ifndef FILEUTILS_
#define FILEUTILS_

#include <string>
#include <fstream>

using namespace std;

bool cscout_input_file(const string &basename, ifstream &in, string &fname);
bool cscout_output_file(const string &basename, ofstream &out, string &fname);
int unlink(const string &name);

#endif // FILEUTILS_
