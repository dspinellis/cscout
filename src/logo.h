/*
 * (C) Copyright 2004-2015 Diomidis Spinellis
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
 * The CScout logo access functions
 *
 */

#ifndef LOGO_
#define LOGO_

#include <cstdio>

using namespace std;

class Logo {
private:
	static const unsigned char logo_bytes[];
	static int len;
public:
	static void logo(FILE *fo) {
		fwrite(logo_bytes, len, 1, fo);
	}
};

#endif // LOGO_
