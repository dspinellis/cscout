/*
 * (C) Copyright 2003-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.24 2011/10/23 16:22:06 dds Exp $
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
#include "ctag.h"

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
	CTag::add(id->get_token(), t.type(), t.get_storage_class());
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
	cfun->definition = t.get_token().get_defining_tokid();
	cfun->defined = true;
	if (DP()) {
		cout << "Current function " << id->get_name() << "\n";
		cout << "Type: " << t << "\n";
	}
	cfun->metrics().set_metric(FunMetrics::em_ngnsoc,
	    Block::global_namespace_occupants_size() +
	    Pdtoken::macros_size());
	nesting.push(cfun);
	if (nesting.size() == 1)
		Fchar::get_fileid().metrics().add_function(t.is_static());
}

