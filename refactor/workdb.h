/*
 * (C) Copyright 1995-2005 Diomidis Spinellis.
 *
 * SQL back-end
 *
 * $Id: workdb.h,v 1.4 2005/06/13 18:10:15 dds Exp $
 */

void workdb_schema(Sql *db, ostream &of);
void workdb_rest(Sql *db, ostream &of);
