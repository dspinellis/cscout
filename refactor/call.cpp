/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: call.cpp,v 1.3 2004/07/24 07:56:06 dds Exp $
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
#include "mcall.h"
#include "eclass.h"

// Function currently being parsed
FCall *Call::current_fun = NULL;

// Set of all functions
Call::fun_map Call::all;

// The current function makes a call to f
void
Call::register_call(Call *f)
{
	if (!current_fun)
		return;
	const Macro *m = Pdtoken::get_body_token_macro(f->get_tokid());
	Call *caller;

	if (m)
		/*
		 * f appears in a macro body, so this is a call
		 * from the macro to the function.
		 */
		caller = m->get_mcall();
	else
		// Function to function call
		caller = current_fun;

	caller->add_call(f);
	f->add_caller(caller);
	if (DP())
		cout << caller->name << " calls " << f->name << "\n";
}

// From calls to
void
Call::register_call(Call *from, Call *to)
{
	from->add_call(to);
	to->add_caller(from);
	if (DP())
		cout << from->name << " calls " << to->name << "\n";
}

// Constructor
Call::Call(const string &s, Tokid t) :
		name(s),
		tokid(t)
{
	all[t] = this;
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
	for (const_fmap_iterator_type i = all.begin(); i != all.end(); i++)
		i->second->visited = false;
}
