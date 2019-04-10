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
 * Standard definitions read when starting up
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 * This file should have read-only permissions
 *
 */

#define __DATE__ ""
#define __TIME__ ""
#define __FILE__ ""
#define __LINE__ 0
#define __PRETTY_FUNCTION__ ""

//#define __cplusplus
#define __STDC__ 1
#define __ELF__
#define unix
#define i386
#define __i386
#define __i386__
#define __tune_i386__
#define linux
#define __linux
#define __linux__
#define __unix
#define __GNUC__ 2
#define __GNUC_MINOR__ 96
#define __WCHAR_TYPE__ short unsigned int

#ifdef __cplusplus
#define __GNUG__ 3
#define __EXCEPTIONS
#endif

#define __extension__
#define __attribute__(x)
#define __const const
#define __builtin_va_list void *
#define __builtin_stdarg_start
#define __builtin_va_end
#define __builtin_va_arg(_ap, _type) (*(_type *)(_ap))
#define __inline__ inline
#define __inline inline
#define __volatile__
