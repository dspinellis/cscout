/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: stab.cpp,v 1.5 2001/09/20 13:15:00 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <cassert>

#include "cpp.h"
#include "debug.h"
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
#include "parse.tab.h"


int Block::current_block = -1;
vectorBlock Block::scope_block;
Stab Function::label;

void
Block::enter()
{
	scope_block.push_back(Block());
	current_block++;
}

void
Block::exit()
{
	scope_block.pop_back();
	current_block--;
}

/*
 * Define name to be the identifier id
 */
void
Block::define(Stab Block::*table, const Token& tok, const Type& typ)
{
	(scope_block[current_block].*table).define(tok, typ);
}

/*
 * Define the tok object to be of type typ
 */
void
obj_define(const Token& tok, const Type& typ)
{
	enum e_storage_class sc = typ.get_storage_class();

	if (sc != c_extern && Block::current_block != Block::cu_block &&
	    Block::scope_block[Block::current_block].obj.lookup(tok.get_name())) {
		Error::error(E_ERR, "Duplicate definition of identifier " + tok.get_name());
		return;
	}
	Block::define(&(Block::obj), tok, typ);
	// Identifiers with extern scope are also added to the linkage unit
	// definitions.  These are not searched, but are used for unification
	if (sc == c_extern || (sc == c_unspecified && Block::current_block == Block::cu_block)) {
		Id const * id;
		if (id = Block::scope_block[Block::lu_block].obj.lookup(tok.get_name()))
			unify(id->get_token(), tok);
		else
			Block::scope_block[Block::lu_block].obj.define(tok, typ);
	}
}

/*
 * Lookup in the block pointed by table (obj or tag) for name
 * and return the relevant identifier or NULL if not defined.
 */
Id const *
Block::lookup(const Stab Block::*table, const string& name)
{
	int i;
	Id const * id;

	for (i = current_block; i != lu_block; i--)
		if (id = (scope_block[i].*table).lookup(name))
			return id;
	return NULL;
}


Id const *
Stab::lookup(const string& s) const
{
	map<string, Id>::const_iterator i;

	 i = m.find(s);
	 if (i == m.end())
	 	return (NULL);
	else
		return &((*i).second);
}

void
Stab::define(const Token& tok, const Type& typ)
{
	m[tok.get_name()] = Id(tok, typ);
}

/*
 * Define tok as a label in the current function.
 * If it is already defined, unify it with the previous definition.
 */
void
label_define(const Token& tok)
{
	Id const *id = Function::label.lookup(tok.get_name());
	if (id) {
		if (id->get_type().is_valid())
			Error::error(E_ERR, "label " + tok.get_name() + " already defined");
		unify(id->get_token(), tok);
	}
	Function::label.define(tok, label());
}

/*
 * Use tok as a label in the current function (with goto).
 * If it is already defined, unify it with the previous definition.
 */
void
label_use(const Token& tok)
{
	Id const *id = Function::label.lookup(tok.get_name());
	if (id)
		unify(id->get_token(), tok);
	else
		Function::label.define(tok, Type());
}

// Called at the end of a function definition
void
Function::exit()
{
	Stab_element::const_iterator i;

	for (i = label.begin(); i != label.end(); i++)
		if (!Stab::get_id(i).get_type().is_valid())
			Error::error(E_ERR, "undefined label " + Stab::get_name(i));
	label.clear();
}
