/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: debug.cpp,v 1.2 2001/09/03 08:13:51 dds Exp $
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
		Debug::db_set(name, line);
		if (DP()) cout << "Enable DP: " << name << "(" << line << ")\n";
	}
}
