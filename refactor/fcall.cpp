/* 
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fcall.cpp,v 1.5 2003/12/04 23:24:46 dds Exp $
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
#include "eclass.h"

// Function currently being parsed
FCall *FCall::current_fun;

// Set of all functions
FCall::fun_container FCall::all;

// Set the funciton currently being parsed
void
FCall::set_current_fun(const Type &t)
{
	Id const *id = obj_lookup(t.get_name());
	assert(id);
	current_fun = id->get_fcall();
	assert(current_fun);
	current_fun->definition = t.get_token().get_parts_begin()->get_tokid();
	current_fun->defined = true;
}

/*
 * Set the funciton currently being parsed
 * This is used for defining yytab, which is not explicitly defined
 */
void
FCall::set_current_fun(const Id *id)
{
	assert(id);
	current_fun = id->get_fcall();
	assert(current_fun);
	current_fun->definition = Tokid();
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
FCall::FCall(const Token& tok, Type typ, const string &s) :
		name(s), 
		declaration(tok.get_parts_begin()->get_tokid()), 
		type(typ),
		defined(false)
{
	all.insert(this);
}

// Return true if e appears in the eclasses comprising out name
bool
FCall::contains(Eclass *e) const
{
	int len = name.length();
	Tokid t = declaration;
	for (int pos = 0; pos < len;) {
		Eclass *e2 = t.get_ec();
		if (e == e2)
			return true;
		t += e2->get_len();
		pos += e2->get_len();
	}
	return false;
}

void
FCall::clear_visit_flags()
{
	for (const_fiterator_type i = all.begin(); i != all.end(); i++)
		(*i)->visited = false;
}
