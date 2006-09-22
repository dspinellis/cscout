/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.15 2006/09/22 20:46:26 dds Exp $
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
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
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

// Nested function definitions
stack <FCall *> FCall::nesting;

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
	nesting.push(current_fun);
}

// Set the function currently being parsed
void
FCall::set_current_fun(const Type &t)
{
	Id const *id = obj_lookup(t.get_name());
	csassert(id);
	current_fun = id->get_fcall();
	if (!current_fun) {
		/*
		 * @error
		 * A non-function type specifier was followed by a block.
		 * Use a function type specifier or add an equals sign before
		 * the block to indicate that the block is an initializer.
		 */
		Error::error(E_ERR, "syntax error: block is not preceded by a function specifier");
		return;
	}
	csassert(current_fun);
	current_fun->definition = t.get_token().get_parts_begin()->get_tokid().unique();
	current_fun->defined = true;
	if (DP())
		cout << "Current function " << id->get_name() << "\n";
	nesting.push(current_fun);
}

void
FCall::unset_current_fun()
{
	nesting.pop();
	if (nesting.empty())
		current_fun = NULL;
	else
		current_fun = nesting.top();
}
