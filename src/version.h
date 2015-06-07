/*
 * (C) Copyright 2003-2015 Diomidis Spinellis
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
 * Access the RCS and global version identifiers
 *
 * Include synopsis: version.h
 *
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
