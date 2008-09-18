/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: attr.cpp,v 1.20 2008/09/18 10:35:42 dds Exp $
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

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"


// Leave space for a single project-attribute
vector<bool>::size_type Attributes::size = attr_end;

int Project::current_projid = attr_end;
int Project::next_projid = attr_end;
// Maps between ids and names
map<string, int> Project::projids;
vector<string> Project::projnames(attr_end);

// Keep this in sync with the enumeration
string Attributes::attribute_names[] = {
	"__attribute(__unused__)",
	"Identifier token from a macro",

	// User-visible attributes start here
	"Read-only",

	"Tag for struct/union/enum",
	"Member of struct/union",
	"Label",
	"Ordinary identifier",

	"Macro",
	"Undefined macro",
	"Macro argument",

	"File scope",
	"Project scope",

	"Typedef",
	"Enumeration constant",
	"Yacc identifier",
	"Function",
};

// Keep this in sync with the enumeration
string Attributes::attribute_short_names[] = {
	"__attribute(__unused__)",
	"idmtoken",

	// User-visible attributes start here
	"ro",

	"tag",
	"member",
	"label",
	"obj",

	"macro",
	"umacro",
	"macroarg",

	"fscope",
	"pscope",

	"typedef",
	"enumconst",
	"yacc",
	"function",
};

void
Project::set_current_project(const string &name)
{
	map<string, int>::const_iterator p;
	if ((p = projids.find(name)) == projids.end()) {
		// Add new project
		current_projid = next_projid++;
		projnames.push_back(name);
		projids[name] = current_projid;
		if (Attributes::get_num_attributes() < projnames.size())
			Attributes::add_attribute();
	} else {
		current_projid = (*p).second;
	}
}
