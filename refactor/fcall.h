/* 
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * Function call graph information
 *
 * $Id: fcall.h,v 1.3 2003/12/04 20:03:08 dds Exp $
 */

#ifndef FCALL_
#define FCALL_

/*
 * Function call information is always associated with Id objects
 * It is thus guaranteed to match the symbol table lookup operations
 */

// C function calling information
class FCall {
private:

	static FCall *current_fun;	// Function currently being parsed

	// Container for storing all declared functions
	typedef set <FCall *> all_container;
	static all_container all;	// Set of all functions

	string name;
	Tokid declaration;		// Function's first declaration 
					// (could also be reference if implicitly declared)
	Tokid definition;		// Function's definition
	Type type;			// Function's type
	set <FCall *> call;		// Functions this function calls
	set <FCall *> caller;		// Functions that call this function
	void add_call(FCall *f) { call.insert(f); }
	void add_caller(FCall *f) { caller.insert(f); }
public:
	// Set the funciton currently being parsed
	static void set_current_fun(const Type &t);
	static void set_current_fun(const Id *id);
	// The current function makes a call to f
	static void register_call(FCall *f);
	// Interface for iterating through all functions
	typedef all_container::const_iterator const_fiterator_type;
	static const_fiterator_type fbegin() { return all.begin(); }
	static const_fiterator_type fend() { return all.end(); }
	Tokid get_declaration() const { return declaration; }
	Tokid get_definition() const { return definition; }
	const string &get_name() const { return name; }
	bool contains(Eclass *e) const;

	FCall(const Token& t, Type typ, const string &s);
};

#endif // FCALL_
