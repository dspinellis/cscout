/*
 * (C) Copyright 2007 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: mcall.cpp,v 1.2 2007/08/12 07:24:06 dds Exp $
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
#include "mcall.h"
#include "macro.h"

void
MCall::set_current_fun(Macro &m)
{
	current_fun = m.get_mcall();
	csassert(current_fun);
	current_fun->mark_begin();
	nesting.push(current_fun);
}

const string &
MCall::entity_type_name() const
{
	static string s("function-like macro");
	return (s);
}
