/*
 * (C) Copyright 1995-2005 Diomidis Spinellis.
 *
 * SQL back-end
 *
 * $Id: workdb.h,v 1.5 2008/12/04 15:19:06 dds Exp $
 */

#ifndef WORKDB_
#define WORKDB_

#include <iostream>

using namespace std;

#include "sql.h"

void workdb_schema(Sql *db, ostream &of);
void workdb_rest(Sql *db, ostream &of);

#endif // WORKDB_
