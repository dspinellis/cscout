/* 
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.2 2003/11/17 20:45:32 dds Exp $
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
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "fdep.h"
#include "fcall.h"

// Function currently being parsed
FCall *FCall::current_fun;

// Set of all functions
FCall::all_container FCall::all;

// Set the funciton currently being parsed
void
FCall::set_current_fun(const Type &t)
{
	Id const *id = obj_lookup(t.get_name());
	assert(id);
	current_fun = id->get_fcall();
	assert(current_fun);
	current_fun->definition = t.get_token();
}

// Set the funciton currently being parsed
void
FCall::set_current_fun(const Id *id)
{
	assert(id);
	current_fun = id->get_fcall();
	assert(current_fun);
	current_fun->definition = Token();
}

// The current function makes a call to f
void
FCall::register_call(FCall *f)
{
	current_fun->add_call(f);
	f->add_caller(current_fun);
	if (DP())
		cout << current_fun->declaration << " calls " << f->declaration << "\n";
}

// Constructor
FCall::FCall(const Token& tok, Type typ) :
		declaration(tok), type(typ)
{
	all.insert(this);
}
