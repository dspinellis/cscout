/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: type.cpp,v 1.1 2001/09/04 13:27:54 dds Exp $
 */

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stack>
#include <deque>
#include <map>
#include <set>

#include "fileid.h"
#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "error.h"
#include "id.h"
#include "type.h"


Type *
Type::subscript()
{
	Error::error(E_ERR, "subscript not on array or pointer");
	return this;
}


Type *
Type::call()
{
	Error::error(E_ERR, "object is not a function");
	return this;
}


Type *
Type::deref()
{
	Error::error(E_ERR, "illegal pointer dereference");
	return this;
}


Type *
Type::type()
{
	Error::error(E_INTERNAL, "object is not a typedef");
	return this;
}


Id *
Type::member(const string& s)
{
	Error::error(E_ERR, "invalid member access: not a structure or union");
	return NULL;
}

Tsu::~Tsu()
{
	map<string, Id *>::const_iterator i;

	for (i = members.begin(); i != members.end(); i++)
		delete (*i).second;
}

Id *
Tsu::member(const string& s) const
{
	map<string, Id *>::const_iterator i;

	return ((i = members.find(s)) == members.end() ? NULL : (*i).second);
}
