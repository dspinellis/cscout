/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: stab.cpp,v 1.18 2002/12/26 12:46:24 dds Exp $
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
#include "metrics.h"
#include "attr.h"
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
#include "parse_tab.h"


int Block::current_block = -1;
vectorBlock Block::scope_block;
Stab Function::label;
Block Block::param_block;	// Function parameter declarations
bool Block::use_param;		// Declare in param_block when true

// Called when entering a scope
void
Block::enter()
{
	scope_block.push_back(Block());
	current_block++;
}

// Called when entering a function block statement
void
Block::param_enter()
{
	if (DP())
		cout << "On param_enter " << param_block.obj << "\n";
	scope_block.push_back(param_block);
	current_block++;
	use_param = false;
}

void
Block::exit()
{
	scope_block.pop_back();
	current_block--;
}

// Called when exiting a function parameter list
void
Block::param_exit()
{
	param_block = scope_block.back();
	scope_block.pop_back();
	current_block--;
	if (DP())
		cout << "On param_exit " << param_block.obj << "\n";
}

/*
 * Define name to be the identifier id
 */
void
Block::define(Stab Block::*table, const Token& tok, const Type& typ)
{
	(scope_block[current_block].*table).define(tok, typ);
}

// Called when exiting a function block statement
void
Block::param_clear(void)
{
	param_block.obj.clear();
	param_block.tag.clear();
}

void
Block::clear()
{
	param_clear();
	scope_block.clear();
	current_block = -1;
}

/*
 * Define the tok object to be of type typ
 */
void
obj_define(const Token& tok, Type typ)
{
	tok.set_ec_attribute(is_ordinary);
	enum e_storage_class sc = typ.get_storage_class();
	Id const * id;

	if (DP())
		cout << "Define object [" << tok.get_name() << "]: " << typ << "\n";
	if (Block::use_param) {
		// Old-style function definition declarations
		// No checking
		if ((id = Block::param_block.obj.lookup(tok.get_name())))
			unify(id->get_token(), tok);
		else
			Error::error(E_ERR, "declared parameter does not appear in old-style function parameter list: " + tok.get_name());
		(Block::param_block.obj).define(tok, typ);
		return;
	}
	if (sc == c_extern && (id = Block::scope_block[Block::cu_block].obj.lookup(tok.get_name()))) {
		// If the declaration of an identifier contains extern the identifier
		// has the same linage as any visible declaration of the identifier
		// with file scope
		enum e_storage_class sc2 = id->get_type().get_storage_class();
		if (sc2 != sc) {
			typ.set_storage_class(basic(b_abstract, s_none, sc2));
			sc = sc2;
		}
	}
	if (DP())
		cout << "Type of " << tok << " is " << typ << (typ.is_typedef() ? " (typedef)\n" : "\n");
	if (Block::get_scope_level() == Block::cu_block) {
		// Special rules for definitions at file scope
		// ANSI 3.1.2.2 p. 22
		if (sc == c_static) {
			tok.set_ec_attribute(is_cscope);
			if ((id = Block::scope_block[Block::cu_block].obj.lookup(tok.get_name()))) {
				if (id->get_type().get_storage_class() == c_unspecified)
					Error::error(E_ERR, "conflicting declarations for identifier " + id->get_name());
				unify(id->get_token(), tok);
			}
		} else if (sc == c_typedef) {
			tok.set_ec_attribute(is_cscope);
			tok.set_ec_attribute(is_typedef);
		} else
			tok.set_ec_attribute(is_lscope);
	} else {
		// Definitions at function block scope
		if (sc != c_extern &&
		    Block::scope_block[Block::current_block].obj.lookup(tok.get_name())) {
			Error::error(E_ERR, "Duplicate definition of identifier " + tok.get_name());
			return;
		}
	}
	static Stab Block::*objptr = &Block::obj;
	Block::define(objptr, tok, typ);
	// Identifiers with extern scope are also added to the linkage unit
	// definitions.  These are not searched, but are used for unification
	if (sc == c_extern || (sc == c_unspecified && Block::current_block == Block::cu_block)) {
		Id const * id;
		if ((id = Block::scope_block[Block::lu_block].obj.lookup(tok.get_name())))
			unify(id->get_token(), tok);
		else
			Block::scope_block[Block::lu_block].obj.define(tok, typ);
	}
}

/*
 * Define the tok struct/union/enum tag to be of type typ
 */
void
tag_define(const Token& tok, const Type& typ)
{
	tok.set_ec_attribute(is_suetag);
	static Stab Block::*tagptr = &Block::tag;
	const Id *id;

	if (DP())
		cout << "Define tag [" << tok.get_name() << "]: " << typ << "\n";
	if (Block::use_param)
		(Block::param_block.tag).define(tok, typ);
	else if ((id = Block::scope_block[Block::current_block].tag.lookup(tok.get_name())) &&
		 !id->get_type().is_incomplete())
		Error::error(E_ERR, "Duplicate definition of tag  " + tok.get_name());
	else
		Block::define(tagptr, tok, typ);
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
		if ((id = (scope_block[i].*table).lookup(name)))
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
	tok.set_ec_attribute(is_label);
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

ostream& 
operator<<(ostream& o,const Stab &s)
{
	Stab_element::const_iterator i;

	o << "{";
	for (i = s.m.begin(); i != s.m.end(); i++)
		o << (*i).first << ": " << ((*i).second.get_type()) << "\n";
	o << "} ";
	return o;
}
