/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: debug.cpp,v 1.5 2009/01/15 14:32:57 dds Exp $
 */

#include <set>
#include <string>
#include <iostream>
#include <fstream>

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

void
Debug::db_read()
{
	ifstream in;
	string name;
	int line;

	in.open("dbpoints");
	if (in.fail())
		return;
	while (!in.eof()) {
		in >> name >> line;
		if (name[0] == '#')
			continue;
		Debug::db_set(name, line);
		if (DP()) cout << "Enable DP: " << name << "(" << line << ")\n";
	}
}
