/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: debug.cpp,v 1.1 2001/09/03 07:43:11 dds Exp $
 */

#include <set>
#include <string>

#include "cpp.h"
#include "debug.h"

bool Debug::enabled;
set<Debug> Debug::dp;		// Enabled debug points

void
Debug::db_set(const string fname, int line)
{
	dp.insert(Debug(fname, line));
	enabled = true;
}
