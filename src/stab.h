/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
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
 * The C symbol table
 *
 */

#ifndef STAB_
#define STAB_

#include <string>

using namespace std;

#include "fileid.h"
#include "token.h"
#include "id.h"
#include "type.h"

/*
 * A refresher on identifier namespaces (ANSI 3.1.2.3)
 * Four namespaces:
 * 1. Label names:
 *    Disambinuated by the syntax of label declaration and
 *    used, stored in Function::label, defined using label_define.
 *    (gcc also allows block-local labels; these are stored in
 *    Block.local_label and defined with local_label_define.)
 * 2. Tags for struct, union, enum:
 *    Disambiguated by following the respective keyword
 *    and stored in Block.tag, defined using tag_define
 * 3. Members of structures and unions
 *    One per structure or union, disambiguated by the type
 *    of the expression used to access it.
 *    Stored in Tsu.members, defined using Tsu ctor or add_member
 * 4. All other objects
 *    AKA ordinary identifiers, stored in Block.obj
 */

class Type;
class FCall;
class GlobObj;

// A C identifier as stored in the symbol table
class Id {
private:
	Token token;			// Any token representing the identifier
					// Needed for unification
	Type type;			// For type resolution
	FCall *fcall;			// Function call info
	GlobObj *glob;			// Corresponding global object
public:
	Id(const Token& tok, Type typ, FCall *fc = NULL, GlobObj *go = NULL);
	// An unnamed identifier (used for unnamed structure members)
	Id(Type typ) : type(typ) {}
	Id() : type(basic(b_undeclared)) {}			// Needed for map
	Type get_type() const { return type; }
	void set_type(const Type &t) { type = t; }
	const Token& get_token() const { return token; }
	const string get_name() const { return token.get_name(); }
	FCall *get_fcall() const { return fcall; }
	GlobObj *get_glob() const { return glob; }
};

typedef map<string,Id> Stab_element;

// A symbol table instance (used (two per block) for objects and tags)
class Stab {
private:
	Stab_element m;
public:
	Id const* lookup(const string& s) const;
	Id *define(const Token& tok, const Type& typ, FCall *fc = NULL, GlobObj *go = NULL);
	void clear() { m.clear(); }
	int size() { return m.size(); }
	Stab_element::const_iterator begin() const { return m.begin(); }
	Stab_element::const_iterator end() const { return m.end(); }
	static const string& get_name(const Stab_element::const_iterator x)
		{ return (*x).first; }
	static const Id& get_id(const Stab_element::const_iterator x)
		{ return (*x).second; }
	void merge_with(const Stab& m2)
		{ m.insert(m2.m.begin(), m2.m.end()); }
	friend ostream& operator<<(ostream& o,const Stab &s);
};

class Block;

typedef vector<Block> vectorBlock;

// Encapsulate symbols with function scope
// Per ANSI these are only the labels
class Function {
private:
	static Stab label;
public:
	// Called when entering or exiting a function
	// Either call will be enough
	static void enter() { label.clear(); }
	static void exit();
	friend void label_define(const Token& tok);
	friend void label_use(const Token& tok);
};

void label_define(const Token& tok);
void label_use(const Token& tok);

void label_define(const Token& tok);
void label_use(const Token& tok);

// A scoped block
class Block {
private:
	static int current_block;	// Current block: >= 1
	static vectorBlock scope_block;
	static Block param_block;	// Function parameter declarations
	static bool param_use;		// Declare in param_block when true
	/*
	 * When dealing with functions returning pointers to functions,
	 * the parameters of the returned function type, which appear later on
	 * in the declaration are not the defined function's parameters.
	 * To handle this set param_seen after seeing the first parameter
	 * block and avoid setting further ones.
	 */
	static bool param_seen;
	static int param_block_nesting;	// Nesting level of defined params

	static void define(Stab Block::*table, const Token& tok, const Type& t, FCall *fc = NULL, GlobObj *go = NULL);
	static pair <Id const *, int> lookup(const Stab Block::*table, const string& name);
	// The file id associated with the compilation unit block
	static Fileid cu_file_id;
public:
	// Should be private appart from taking member address
	Stab obj;		// Objects (variables...)
	Stab tag;		// Aggregate (struct, union) tags
	Stab local_label;	// Local labels; gcc extension

	static int get_scope_level() { return current_block; }
	static void set_scope_level(int level) { current_block = level; }
	static const int lu_block = 0;	// Linkage unit definitions: 0
	static const int cu_block = 1;	// Compilation unit definitions: 1
	static void enter();
	static void exit();
	static void clear();		// Clear all block-related information
	// Set Fileid of compilation unit being processed
	static void set_cu_file_id(Fileid id) { cu_file_id = id; }
	// Return the number of namespace occupants of the cu and lu blocks
	static int global_namespace_occupants_size() {
		return scope_block[Block::lu_block].obj.size() +
		    scope_block[Block::cu_block].obj.size();
	}

	/*
	 * Special block for handling function parameters.
	 * Function parameters can appear in parameter_type_lists or in
	 * identifier lists.  These are entered as usual with Block::enter()
	 * to define a new scope.  However, they are exited with
	 * Block::param_exit() which saves a copy of the block in param_block.
	 * (Note that nested scopes are correctly ignored).
	 * The function block starts with Block::fn_body_enter() which copies
	 * the last saved param_block into the current block and calls
	 * Block::param_clear().
	 * All definitions and declarations call Block::param_clear() to keep
	 * the, possibly contaminated, param_block fresh for new function
	 * definitions.
	 * Old-style function parameter declarations occur outside the function
	 * parameter list and would therefore be defined at file scope.
	 * Calling Block::param_use_begin() instructs the symbol table to add all
	 * definitions in param_block instead. This special state is
	 * automatically cleared when entering the function body with
	 * Block::fn_body_enter().
	 *
	 * The following diagram depicts where each function is called:
	 *
	 * int newfun(@enter@int i, void (bar)(@enter@void@param_exit@)@param_exit@)
	 * {@fn_body_enter@
	 * 	return i;
	 * }@exit,param_clear@
	 *
	 * oldfun(@enter@a, b@param_exit@)
	 * 	@param_use_begin@
	 * 	int a;
	 * 	double b;
	 * 	@param_use_end@
	 * {@fn_body_enter@
	 * 	return i;
	 * }@exit,param_clear@
	 *
	 * Furthermore, do not use as parameters those of nested
	 * function argument declarations and the declared arguments 
	 * of returned function pointers.
	 * Nested arguments are ignored through param_block_nesting.
	 * Arguments of returned function pointers are ignored through
	 * param_seen, because they always appear later in the definition.
	 * See test/c/c42-funargptr.c
	 */
	static void fn_body_enter();
	static void param_exit();
	static void param_clear(void);
	static void param_use_begin(void) { param_use = true; }
	static void param_use_end(void) { param_use = false; }

	static int get_cur_block() { return current_block; }

	// Lookup and define of objects and struct/union/enum tags
	friend Id const * obj_lookup(const string& name);
	friend void obj_define(const Token& tok, Type t);
	inline friend Id const * tag_lookup(const string& name);
	friend void tag_define(const Token& tok, const Type& t);
	friend void fix_incomplete(const Token& tok, const Type& t);
	inline friend Id const * local_label_lookup(const string& name);
	friend void label_define(const Token& tok);
	friend void local_label_define(const Token& tok);
	inline friend Id const * tag_lookup(int block_level, const string& name);
};

Id const * obj_lookup(const string& name);
void obj_define(const Token& tok, Type t);
void tag_define(const Token& tok, const Type& t);
void local_label_define(const Token& tok);

inline Id const *
tag_lookup(const string& name)
{
	static Stab Block::*tagptr = &Block::tag;
	return Block::lookup(tagptr, name).first;
}

inline Id const *
local_label_lookup(const string& name)
{
	static Stab Block::*llptr = &Block::local_label;
	return Block::lookup(llptr, name).first;
}

inline Id const *
tag_lookup(int block_level, const string& name)
{
	csassert(Block::current_block >= block_level);
	return Block::scope_block[block_level].tag.lookup(name);
}
#endif // STAB_
