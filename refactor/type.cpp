/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: type.cpp,v 1.3 2001/09/13 16:34:37 dds Exp $
 */

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <list>

#include "ytab.h"

#include "fileid.h"
#include "fileid.h"
#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "error.h"
#include "ptoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"



Type&
Type::operator=(const Type& rhs)
{
	// The operation sequence handles self-assignment
	rhs.p->use++;
	if (--p->use == 0)
		delete p;
	p = rhs.p;
	return (*this);
}

Type
Type_node::subscript() const
{
	Error::error(E_ERR, "subscript not on array or pointer");
	return basic(b_undeclared);
}


Type
Type_node::call() const
{
	Error::error(E_ERR, "object is not a function");
	return basic(b_undeclared);
}

Type
Tbasic::call() const
{
	// Undeclared identifiers f when called are declared as int f(...)
	if (type == b_undeclared) {
		obj_define(this->get_token(), function_returning(basic(b_int)));
		return basic(b_int);
	} else {
		Error::error(E_ERR, "object is not a function");
		return basic(b_undeclared);
	}
}


const Ctoken& 
Type_node::get_token() const
{
	static Ctoken c;
	Error::error(E_INTERNAL, "attempt to get token value of a non-identifier");
	return c;
}

const string&
Type_node::get_name() const
{
	static string s("unknown");
	Error::error(E_INTERNAL, "attempt to get the name of a non-identifier");
	return s;
}

Type
Type_node::deref() const
{
	Error::error(E_ERR, "illegal pointer dereference");
	return basic(b_undeclared);
}


Type
Type_node::type() const
{
	Error::error(E_INTERNAL, "object is not a typedef");
	return basic(b_undeclared);
}


Id
Type_node::member(const string& s) const
{
	Error::error(E_ERR, "invalid member access: not a structure or union");
	return Id(Token(), basic(b_undeclared));
}

Id
Tsu::member(const string& s) const
{
	map<string, Id>::const_iterator i;

	if ((i = members.find(s)) == members.end()) {
		Error::error(E_ERR, "structure or union does not have a member " + s);
		return Id(Token(), basic(b_undeclared));
	} else
		return ((*i).second);
}
Type
basic(enum e_btype t = b_abstract, enum e_sign s = s_none)
{
	return Type(new Tbasic(t, s));
}

Type
array_of(Type t)
{
	return Type(new Tarray(t));
}

Type
pointer_to(Type t)
{
	return Type(new Tpointer(t));
}

Type
function_returning(Type t)
{
	return Type(new Tfunction(t));
}

Type
typedef_for(Type t)
{
	return Type(new Ttypedef(t));
}

Type
enum_tag()
{
	return Type(new Ttag(tt_enum));
}

Type
struct_tag()
{
	return Type(new Ttag(tt_struct));
}

Type
union_tag()
{
	return Type(new Ttag(tt_union));
}

Id
Type::member(const string& name) const
{
	return p->member(name);
}

void
Tsu::add_member(string& name, Id i)
{
	members[name] = i;
}

Type
identifier(const Ctoken& t)
{
	return Type(new Tidentifier(t));
}

void
Tbasic::print(ostream &o) const
{
	switch (sign) {
	case s_none: break;
	case s_signed: o << "signed "; break;
	case s_unsigned: o << "unsigned "; break;
	}

	switch (type) {
	b_abstract: o << "ABSTRACT "; break;
	b_void: o << "void "; break;
	b_char: o << "char "; break;
	b_short: o << "short "; break;
	b_int: o << "int "; break;
	b_long: o << "long "; break;
	b_float: o << "float "; break;
	b_double: o << "double "; break;
	b_ldouble: o << "long double "; break;
	b_undeclared: o << "UNDECLARED "; break;
	b_typedef: o << "typedef "; break;
	}
}

void
Tarray::print(ostream &o) const
{
	o << "array of " << of;
}

void
Tpointer::print(ostream &o) const
{
	o << "pointer to " << to;
}

void
Tfunction::print(ostream &o) const
{
	o << "function returning " << returning;
}

void
Ttypedef::print(ostream &o) const
{
	o << "typedef for " << for_type;
}

void
Ttag::print(ostream &o) const
{
	switch (type) {
	case tt_struct: o << "struct "; break;
	case tt_union: o << "union "; break;
	case tt_enum: o << "enum "; break;
	}
}

void
Tsu::print(ostream &o) const
{
	map<string,Id>::const_iterator i;

	o << "{";
	for (i = members.begin(); i != members.end(); ) {
		o << (*i).first << ": " << ((*i).second.get_type());
		i++;
		if (i != members.end())
			o << ", ";
	}
	o << "} ";
}

void
Tidentifier::print(ostream &o) const
{
	o << t;
}
