/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: attr.cpp,v 1.3 2002/09/17 07:55:39 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <cassert>

#include "cpp.h"
#include "debug.h"
#include "metrics.h"
#include "fileid.h"
#include "attr.h"


// Leave space for a single project-attribute
int Attributes::size = attr_max + 1;

int Project::projid = attr_max - 1;
// Maps between ids and names
map<string, int> Project::projids;
vector<string> Project::projnames(attr_max);


void
Project::set_current_project(const string &name)
{
	map<string, int>::const_iterator p;
	if ((p = projids.find(name)) == projids.end()) {
		// Add new project
		projid++;
		projnames.push_back(name);
		projids[name] = projid;
		Attributes::add_attribute();
	} else {
		projid = (*p).second;
	}
}
