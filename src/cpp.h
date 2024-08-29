/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * Various CScout utility definitions.
 *
 */

#ifndef CPP_H
#define CPP_H

#include <cstddef>

using namespace std;

// The offset we use for files
typedef long cs_offset_t;

/*
 * Return as an integral value a pointer's offset (in units of T) for
 * writing it to a database.
 * Not strictly portable, but better than casting to unsigned.
 */
template <class T>
inline ptrdiff_t
ptr_offset(T p)
{
	return p - (T)0;
}

#ifdef NDEBUG
#define ASSERT(x) ((void)0)
#else
#define ASSERT(x) (x ? (void)0 : (Error::error(E_INTERNAL, "Failed assertion"), assert(x)))
#endif


#endif /* CPP_H */
