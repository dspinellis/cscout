/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The C symbol table
 *
 * $Id: stab.h,v 1.11 2002/09/07 09:47:15 dds Exp $
 */

#ifndef STAB_
#define STAB_

class Type;

// An C identifier as stored in the symbol table
class Id {
private:
	Token token;			// Any token representing the identifier
					// Needed for unification
	Type type;			// For type resolution
public:
	Id(const Token& tok, Type typ) :
		token(tok), type(typ) {}
	Id() : type(basic(b_undeclared)) {}			// Needed for map
	Type get_type() const { return type; }
	const Token& get_token() const { return token; }
	const string& get_name() const { return token.get_name(); }
};

typedef map<string,Id> Stab_element;

// A symbol table instance (used (two per block) for objects and tags)
class Stab {
private:
	Stab_element m;
public:
	Id const* lookup(const string& s) const;
	void define(const Token& tok, const Type& typ);
	void clear() { m.clear(); }
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

class Block {
private:
	static int current_block;	// Current block: >= 1
	static vectorBlock scope_block;
	static Block param_block;	// Function parameter declarations
	static bool use_param;		// Declare in param_block when true

	static void define(Stab Block::*table, const Token& tok, const Type& t);
	static Id const * lookup(const Stab Block::*table, const string& name);
public:
	// Should be private appart from taking member address
	Stab obj;
	Stab tag;

	static int get_scope_level() { return current_block; }
	static const int lu_block = 0;	// Linkage unit definitions: 0
	static const int cu_block = 1;	// Compilation unit definitions: 1
	static void enter();
	static void exit();
	static void clear();		// Clear all block-related information

	/*
	 * Special block for handling function parameters.
	 * Function parameters can appear in parameter_type_lists or in 
	 * identifier lists.  These are entered as usual with Block::enter()
	 * to define a new scope.  However, they are exited with 
	 * Block::param_exit() which saves a copy of the block in param_block.
	 * (Note that nested scopes are correctly ignored).
	 * The function block starts with Block::param_enter() which copies
	 * the last saved param_block into the current block.
	 * All definitions and declarations call Block::param_clear() to keep
	 * the, possibly contaminated, param_blockfresh for new function
	 * definitions.
	 * Old-style function parameter declarations occur outside the function
	 * parameter list and would therefore be defined at file scope.
	 * Calling Block::param_use() instructs the symbol table to add all
	 * definitions in param_block instead. This special state is
	 * automatically cleared when entering the function body with
	 * Block::param_enter().
	 *
	 * The following diagram depicts where each function is called:
	 * *
	 * int newfun(@enter@int i, void (bar)(@enter@void@param_exit@)@param_exit@)
	 * {@param_enter@
	 * 	return i;
	 * }@exit,param_clear@
	 *
	 * oldfun(@enter@a, b@param_exit@)
	 * 	@param_use@
	 * 	int a;
	 * 	double b;
	 * 	@param_use_end@
	 * {@param_enter@
	 * 	return i;
	 * }@exit,param_clear@
	 */
	static void param_enter();
	static void param_exit();
	static void param_clear(void);
	static void param_use(void) { use_param = true; }
	static void param_use_end(void) { use_param = false; }

	static int get_cur_block() { return current_block; }

	// Lookup and define of objects and struct/union/enum tags
	inline friend Id const * obj_lookup(const string& name);
	friend void obj_define(const Token& tok, Type t);
	inline friend Id const * tag_lookup(const string& name);
	friend void tag_define(const Token& tok, const Type& t);
	friend void fix_incomplete(const Token& tok, const Type& t);
	inline friend Id const * tag_lookup(int block_level, const string& name);
};


inline Id const *
obj_lookup(const string& name)
{
	static Stab Block::*objptr = &Block::obj;
	return Block::lookup(objptr, name);
}


inline Id const *
tag_lookup(const string& name)
{
	static Stab Block::*tagptr = &Block::obj;
	return Block::lookup(tagptr, name);
}

inline Id const * tag_lookup(int block_level, const string& name)
{
	assert(Block::current_block >= block_level);
	return Block::scope_block[block_level].tag.lookup(name);
}
#endif // STAB_
