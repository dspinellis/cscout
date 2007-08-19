/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.21 2007/08/19 09:07:54 dds Exp $
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

// Constructor
FCall::FCall(const Token& tok, Type typ, const string &s) :
		Call(s, tok),
		type(typ),
		defined(false)
{
}

/*
 * Set the function currently being parsed
 * This is used for defining yytab, which is not explicitly defined
 */
void
FCall::set_current_fun(const Id *id)
{
	csassert(id);
	FCall *cfun;			// FCall rather than simply Call
	current_fun = cfun = id->get_fcall();
	csassert(current_fun);
	current_fun->mark_begin();
	cfun->definition = Tokid();
	cfun->metrics().set_metric(FunMetrics::em_ngnsoc,
	    Block::global_namespace_occupants_size() +
	    Pdtoken::macros_size());
	nesting.push(current_fun);
}

// Set the (C) function currently being parsed
void
FCall::set_current_fun(const Type &t)
{
	Id const *id = obj_lookup(t.get_name());
	csassert(id);
	FCall *cfun;			// FCall rather than simply Call
	current_fun = cfun = id->get_fcall();
	if (!cfun) {
		/*
		 * @error
		 * A non-function type specifier was followed by a block.
		 * Use a function type specifier or add an equals sign before
		 * the block to indicate that the block is an initializer.
		 */
		Error::error(E_ERR, "syntax error: block is not preceded by a function specifier");
		return;
	}
	csassert(cfun);
	cfun->mark_begin();
	cfun->definition = t.get_token().get_parts_begin()->get_tokid().unique();
	cfun->defined = true;
	if (DP())
		cout << "Current function " << id->get_name() << "\n";
	cfun->metrics().set_metric(FunMetrics::em_ngnsoc,
	    Block::global_namespace_occupants_size() +
	    Pdtoken::macros_size());
	nesting.push(cfun);
	if (nesting.size() == 1)
		Fchar::get_fileid().metrics().add_function(t.is_static());
}

