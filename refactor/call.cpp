/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: call.cpp,v 1.1 2004/07/23 06:21:32 dds Exp $
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
#include "call.h"
#include "fcall.h"
#include "eclass.h"

// Function currently being parsed
FCall *Call::current_fun = NULL;

// Set of all functions
Call::fun_container Call::all;

// The current function makes a call to f
void
Call::register_call(Call *f)
{
	if (!current_fun)
		return;
	current_fun->add_call(f);
	f->add_caller(current_fun);
	if (DP())
		cout << current_fun->name << " calls " << f->name << "\n";
}

// Constructor
Call::Call(const string &s) :
		name(s)
{
}

// Return true if e appears in the eclasses comprising our name
bool
Call::contains(Eclass *e) const
{
	int len = name.length();
	Tokid t = get_tokid();
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
Call::clear_visit_flags()
{
	for (const_fiterator_type i = all.begin(); i != all.end(); i++)
		(*i)->visited = false;
}
