/*
 * (C) Copyright 1995-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
