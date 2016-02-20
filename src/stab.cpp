/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * For documentation read the corresponding .h file
 *
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
#include "parse.tab.h"
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
#include "mcall.h"
#include "globobj.h"
#include "ctag.h"


int Block::current_block = -1;
vectorBlock Block::scope_block;
Stab Function::label;
Block Block::param_block;	// Function parameter declarations
bool Block::use_param;		// Declare in param_block when true

Id::Id(const Token& tok, Type typ, FCall *fc, GlobObj *go) :
	token(tok), type(typ), fcall(fc), glob(go)
{
}

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
	// Do not clobber parameter block when exiting function pointer
	// arguments appearing in old-style argument declarations
	if (!use_param)
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
Block::define(Stab Block::*table, const Token& tok, const Type& typ, FCall *fc, GlobObj *go)
{
	(scope_block[current_block].*table).define(tok, typ, fc, go);
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
	if (Block::use_param && Block::current_block == Block::cu_block) {
		// Old-style function definition declarations
		// No checking
		if ((id = Block::param_block.obj.lookup(tok.get_name())))
			Token::unify(id->get_token(), tok);
		else
			/*
			 * @error
			 * While processing an old-style (K&amp;R) parameter
			 * declaration, a declared parameter did not match
			 * any of the parameters appearing in the function's
			 * arguments definition
			 */
			Error::error(E_ERR, "declared parameter does not appear in old-style function parameter list: " + tok.get_name());
		(Block::param_block.obj).define(tok, typ);
		return;
	}
	if (sc == c_unspecified && typ.is_cfunction())
		// If the declaration of an identifier for a function has no
		// storage-class specifier, its linkage is determined exactly
		// as if it were declared with the storage class specifier
		// extern 6.2.2-5
		sc = c_extern;
	if (sc == c_extern && (id = obj_lookup(tok.get_name()))) {
		// If the declaration of an identifier contains extern the identifier
		// has the same linkage as the prior visible declaration of the identifier
		// 6.2.2-4
		enum e_storage_class sc2 = id->get_type().get_storage_class();
		if (sc2 != sc) {
			typ.set_storage_class(basic(b_abstract, s_none, sc2));
			sc = sc2;
		}
	}
	if (DP())
		cout << "Type of " << tok << " is " << typ << (typ.is_typedef() ? " (typedef)\n" : "\n");
	if (Block::get_scope_level() == Block::cu_block) {
		// Function *definitions* are added from FCall::set_current_fun
		// We don't add extern variables
		if (!typ.is_cfunction() && sc != c_extern)
			CTag::add(tok, typ, sc);

		// Special rules for definitions at file scope
		// ANSI 3.1.2.2 p. 22
		switch (sc) {
		case c_static:
			tok.set_ec_attribute(is_cscope);
			if ((id = Block::scope_block[Block::cu_block].obj.lookup(tok.get_name()))) {
				if (id->get_type().get_storage_class() == c_unspecified)
					/*
					 * @error
					 * An identifier is declared twice
					 * with compilation or linkage
					 * unit scope with conflicting
					 * declarations
					 */
					Error::error(E_ERR, "conflicting declarations for identifier " + id->get_name());
				Token::unify(id->get_token(), tok);
			}
			if (!typ.is_cfunction())
				Fchar::get_fileid().metrics().add_fvar();
			break;
		case c_typedef:
			tok.set_ec_attribute(is_cscope);
			tok.set_ec_attribute(is_typedef);
			break;
		case c_enum:
			tok.set_ec_attribute(is_cscope);
			tok.set_ec_attribute(is_enumeration);
			break;
		default:
			if (!typ.is_cfunction())
				Fchar::get_fileid().metrics().add_pvar();
			tok.set_ec_attribute(is_lscope);
			break;
		}
		// A definition contributing data to the current CU
		if (sc != c_extern && sc != c_typedef && sc != c_enum && !typ.is_cfunction()) {
			if (DP())
				Error::error(E_DEBUG, "Add provider through obj_define");
			Fdep::add_provider(Fchar::get_fileid());
		}
	} else {
		// Definitions at function block scope
		if (sc != c_extern &&
		    Block::scope_block[Block::current_block].obj.lookup(tok.get_name())) {
			/*
			 * @error
			 * An identifier is declared twice within the
			 * same block
			 */
			Error::error(E_ERR, "Duplicate definition of identifier " + tok.get_name());
			return;
		}
	}
	// Locate/create the appropriate FCall object
	FCall *fc = NULL;
	if (typ.is_cfunction() && !typ.is_typedef()) {
		if (DP())
			cout << "Looking for function " << tok.get_name() << '\n';
		tok.set_ec_attribute(is_cfunction);
		if (sc == c_extern || (sc == c_unspecified && Block::current_block == Block::cu_block)) {
			// Extern linkage: get it from the lu block which we do not normaly search
			if ((id = Block::scope_block[Block::lu_block].obj.lookup(tok.get_name())) != NULL)
				fc = id->get_fcall();
		} else {
			// Static linkage: get it from the normal blocks
			if ((id = obj_lookup(tok.get_name())) != NULL)
				fc = id->get_fcall();
		}
		// Try to match the function against one in another project
		if (!fc) {
			Token utok(tok.unique());	// To handle identical files
			fc = dynamic_cast<FCall *>(Call::get_call(utok));
			if (!fc) {
				if (DP())
					cout << "Creating new call\n";
				fc = new FCall(utok, typ, tok.get_name());
			}
		}
		fc->metrics().set_metric(FunMetrics::em_nparam, typ.get_nparam());
	}

	static Stab Block::*objptr = &Block::obj;
	Block::define(objptr, tok, typ, fc);
	/*
	 * Identifiers with extern scope are also added to the linkage unit
	 * definitions.  These definitions are not searched, for locating objects,
	 * but are used for unification of objects, simulating the linking phase.
	 */
	if (sc == c_extern || (sc == c_unspecified && Block::current_block == Block::cu_block)) {
		GlobObj *go = NULL;
		if ((id = Block::scope_block[Block::lu_block].obj.lookup(tok.get_name())) != NULL) {
			Token::unify(id->get_token(), tok);
			go = id->get_glob();
		} else {
			/*
			 * Create all the GlobObj elements we track at their declaration/definition point,
			 * so that id is correctly initialized with one.  However, we add a def, only if it
			 * is a definition of a global variable, rather than a declaration.
			 */
			if (!typ.is_cfunction()) {
				if (DP())
					cout << "Define global variable identifier " << tok.get_name() << endl;
				// Try to match the global against one in another project
				Token utok(tok.unique());	// To handle identical files
				go = GlobObj::get_glob(utok);
				if (!go) {
					if (DP())
						cout << "Creating new glob\n";
					go = new GlobObj(utok, typ, tok.get_name());
				}
			}
			id = Block::scope_block[Block::lu_block].obj.define(tok, typ, fc, go);
		}
		/*
		 * We test go, because it might be null if the object is defined as a function in one
		 * and as a variable in another.
		 */
		if (go && !typ.is_cfunction() && sc == c_unspecified && Block::current_block == Block::cu_block)
			go->add_def(Fchar::get_fileid());
	}
}

/*
 * Define the tok struct/union/enum tag to be of type typ
 */
void
tag_define(const Token& tok, const Type& typ)
{
	// Store CTags info
	CTag::add(tok, typ);
	// For structures and unions (but not for enums) store their members
	if (typ.is_su() && !typ.is_incomplete()) {
		const vector <Id>& members = typ.get_members_by_ordinal();
		for (vector <Id>::const_iterator i = members.begin(); i != members.end(); i++)
			CTag::add(i->get_token(), typ.ctags_kind(), tok.get_name());
	}

	// Update symbol table
	tok.set_ec_attribute(is_suetag);
	static Stab Block::*tagptr = &Block::tag;
	const Id *id;

	if (DP())
		cout << "Define tag [" << tok.get_name() << "]: " << typ << "\n";
	if (Block::use_param && Block::current_block == Block::cu_block)
		(Block::param_block.tag).define(tok, typ);
	else if ((id = Block::scope_block[Block::current_block].tag.lookup(tok.get_name())) &&
		 !id->get_type().is_incomplete())
		/*
		 * @error
		 * A structure, union, or enumeration tag was defined
		 * twice for the same entity
		 */
		Error::error(E_ERR, "Duplicate definition of tag  " + tok.get_name());
	else
		Block::define(tagptr, tok, typ);
}


/*
 * Lookup in the block pointed by table (obj or tag) for name
 * and return the relevant identifier or NULL if not defined
 * and the definition's scope level.
 */
pair <Id const *, int>
Block::lookup(const Stab Block::*table, const string& name)
{
	Id const * id;

	for (int i = current_block; i != lu_block; i--)
		if ((id = (scope_block[i].*table).lookup(name)))
			return pair <Id const *, int>(id, i);
	return pair <Id const *, int>(NULL, 0);
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


Id const *
obj_lookup(const string& name)
{
	static Stab Block::*objptr = &Block::obj;
	pair <Id const *, int> r = Block::lookup(objptr, name);
	Id const *id = r.first;
	if (id) {
		enum e_storage_class sc = r.first->get_type().get_storage_class();
		if (!r.first->get_type().is_cfunction() &&
		    (sc == c_extern || (sc == c_unspecified && r.second == Block::cu_block))) {
			if (DP())
				cout << "Lookup global variable identifier " << name << endl;
			GlobObj *go = id->get_glob();
			// Try to match the global against one in another project
			if (!go) {
				Token utok(id->get_token().unique());	// To handle identical files
				go = GlobObj::get_glob(utok);
				if (!go) {
					if (DP())
						cout << "Creating new glob\n";
					go = new GlobObj(utok, id->get_type(), name);
				}
			}
			go->add_ref(Fchar::get_fileid());
		}
	}
	return id;
}


Id *
Stab::define(const Token& tok, const Type& typ, FCall *fc, GlobObj *go)
{
	Stab_element::key_type key(tok.get_name());
	Stab_element::mapped_type data(Id(tok, typ, fc, go));
	/*
	 * Efficient implementation of
	 * m[tok.get_name()] = Id(tok, typ, fc, go);
	 * return (Id *)Stab::lookup(tok.get_name());
	 * See Meyers Effective STL, Item 24.
	 */
	Stab_element::iterator i = m.lower_bound(key);
	if (i != m.end() && i->first == key) {
		i->second = data;
		return &(i->second);
	} else
		return &(m.insert(i, Stab_element::value_type(key, data))->second);
}

/*
 * Define a local label (gcc extension)
 */
void
local_label_define(const Token& tok)
{
	tok.set_ec_attribute(is_label);
	static Stab Block::*llptr = &Block::local_label;
	const Id *id;

	if (DP())
		cout << "Define local label [" << tok.get_name() << "\n";
	if ((id = Block::scope_block[Block::current_block].local_label.lookup(tok.get_name())))
		/*
		 * @error
		 * A local label was defined more than once in the same block
		 */
		Error::error(E_ERR, "Duplicate local label definition " + tok.get_name());
	else
		Block::define(llptr, tok, Type());
}

/*
 * Define tok as a label in the current function.
 * If it is already defined, unify it with the previous definition.
 * Locally defined labels take precedence
 */
void
label_define(const Token& tok)
{
	tok.set_ec_attribute(is_label);
	bool is_local;
	static Stab Block::*llptr = &Block::local_label;

	Id const *id;
	// Search first for local, then for function label
	if ((id = local_label_lookup(tok.get_name())))
		is_local = true;
	else {
		id = Function::label.lookup(tok.get_name());
		is_local = false;
	}
	if (id) {
		if (id->get_type().is_valid())
			/*
			 * @error
			 * The same <code>goto</code> label is defined more
			 * than once in a given function
			 */
			Error::error(E_ERR, "label " + tok.get_name() + " already defined");
		Token::unify(id->get_token(), tok);
	}
	if (is_local)
		Block::define(llptr, tok, label());
	else
		Function::label.define(tok, label());
}

/*
 * Use tok as a label in the current function (with goto).
 * If it is already defined, unify it with the previous definition.
 * Local labels take precedence in searching
 */
void
label_use(const Token& tok)
{
	Id const *id;
	if ((id = local_label_lookup(tok.get_name())) == NULL)
		id = Function::label.lookup(tok.get_name());
	if (id)
		Token::unify(id->get_token(), tok);
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
			/*
			 * @error
			 * A <code>goto</code>
			 * label used within a function was never defined
			 */
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
