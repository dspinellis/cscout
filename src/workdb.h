/*
 * (C) Copyright 1995-2015 Diomidis Spinellis
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
 * SQL back-end
 *
 */

#ifndef WORKDB_
#define WORKDB_

#include <iostream>

using namespace std;

#include "sql.h"

void workdb_schema(Sql *db, ostream &of);
void workdb_rest(Sql *db, ostream &of);

#endif // WORKDB_
