/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: attr.cpp,v 1.1 2002/09/15 15:45:29 dds Exp $
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
#include "fileid.h"
#include "attr.h"


// Leave space for a single project-attribute
int Attributes::size = attr_max + 1;
vector<string> Attributes::names(attr_max + 1);
