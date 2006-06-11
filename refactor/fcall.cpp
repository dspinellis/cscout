/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.11 2006/06/11 21:44:18 dds Exp $
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
#include "call.h"
#include "fcall.h"
#include "eclass.h"


// Constructor
FCall::FCall(const Token& tok, Type typ, const string &s) :
		Call(s, tok),
		type(typ),
		defined(false)
{
}

/*
 * Set the funciton currently being parsed
 * This is used for defining yytab, which is not explicitly defined
 */
void
FCall::set_current_fun(const Id *id)
{
	csassert(id);
	current_fun = id->get_fcall();
	csassert(current_fun);
	current_fun->definition = Tokid();
}

// Set the function currently being parsed
void
FCall::set_current_fun(const Type &t)
{
	Id const *id = obj_lookup(t.get_name());
	csassert(id);
	current_fun = id->get_fcall();
	csassert(current_fun);
	current_fun->definition = t.get_token().get_parts_begin()->get_tokid();
	current_fun->defined = true;
	if (DP())
		cout << "Current function " << id->get_name() << "\n";
}
