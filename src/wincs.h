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
 * Define include paths
 * Windows/gcc (mingw) version
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 *
 */

#ifdef __cplusplus
#pragma includepath "/gcc/include/c++/3.2"
#endif

#pragma includepath "/gcc/include"
#pragma includepath "/gcc/lib/gcc-lib/mingw32/3.2/include"
#pragma ro_prefix "/gcc/include"
#pragma ro_prefix "/gcc/lib/gcc-lib/mingw32/3.2/include"
/* To avoid unused include file warnings */
static void _cscout_dummy2(void) { _cscout_dummy2(); }
