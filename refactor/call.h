/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information
 *
 * $Id: call.h,v 1.10 2004/08/07 21:49:01 dds Exp $
 */

#ifndef CALL_
#define CALL_

class FCall;

/*
 * Generic call information of a called/calling entity.
 * The calls be to/from macros or functions.
 * We identify four types of calls:
 * 1. Macro calls macro
 *	Function like macro-expansion while expanding a macro
 * 2. Macro calls function
 *	parse.y type.cpp call register_call(id->get_fcall())
 *	Function call with the identifier token being part of a
 *	(function-like) macro boy
 * 3. Function calls function
 *	parse.y type.cpp call register_call(id->get_fcall())
 *	Function call with the identifier token not part of a
 *	(function-like) macro body
 * 4. Function calls macro
 *	Function-like macro expanded while processing a function body
 * This scheme does not handle:
 *	macros that expand to functions
 *	functions containing function names that are object-like macros
 *	macro names consisting of multiple parts
 */
class Call {
private:

	// Container for storing all declared functions
	typedef set <Call *> fun_container;
	typedef multimap <Tokid, Call *> fun_map;

	string name;			// Function's name
	fun_container call;		// Functions this function calls
	fun_container caller;		// Functions that call this function
	bool visited;			// For calculating transitive closures
	void add_call(Call *f) { call.insert(f); }
	void add_caller(Call *f) { caller.insert(f); }

protected:
	static fun_map all;	// Set of all functions
	static FCall *current_fun;	// Function currently being parsed

	/*
	 * A token (almost) uniquely identifying the call entity.
	 * Examples:
	 * Function's first declaration
	 * (could also be reference if implicitly declared)
	 * Macro's definition
	 */
	Token token;

public:
	// The current function makes a call to id
	static void register_call(const Id *id);
	// The current function makes a call to f
	static void register_call(Call *f);
	// The current function (token t) makes a call to f
	static void register_call(const Token &t, const Id *id);
	// The current function (tokid t) makes a call to f
	static void register_call(Tokid t, Call *f);

	// A call from from to to
	static void register_call(Call *from, Call *to);

	// Clear the visit flags for all functions
	static void clear_visit_flags();

	// Interface for iterating through all functions
	typedef fun_map::const_iterator const_fmap_iterator_type;
	static const_fmap_iterator_type fbegin() { return all.begin(); }
	static const_fmap_iterator_type fend() { return all.end(); }
	static int fsize() { return all.size(); }
	static const fun_map &functions() { return all; }

	// Get a call site for a given Token
	static Call *get_call(const Token &t);
	// Get a call sites for a given Tokid
	static pair <const_fmap_iterator_type, const_fmap_iterator_type> get_calls(Tokid t);

	const string &get_name() const { return name; }
	bool contains(Eclass *e) const;

	// Interface for iterating through calls and callers
	typedef fun_container::const_iterator const_fiterator_type;
	const_fiterator_type call_begin() const { return call.begin(); }
	const_fiterator_type call_end() const { return call.end(); }
	const_fiterator_type caller_begin() const { return caller.begin(); }
	const_fiterator_type caller_end() const { return caller.end(); }

	int get_num_call() const { return call.size(); }
	int get_num_caller() const { return caller.size(); }

	void set_visited() { visited = true; }
	bool is_visited() const { return visited; }

	// Return a token for the given object
	const Token &get_token() const {return token; }
	// Return a tokid for the given object
	Tokid get_tokid() const {return token.get_parts_begin()->get_tokid(); }
	// Return the function's file-id
	Fileid get_fileid() const {return token.get_parts_begin()->get_tokid().get_fileid(); }

	// Return true if the function is defined
	virtual bool is_defined() const = 0;
	// Return true if the function is declared
	virtual bool is_declared() const = 0;
	// Return the name of the entity's type
	virtual const string & entity_type_name() const = 0;
	// Return true if the function is static (e.g. a macro or a static C function)
	virtual bool is_file_scoped() const = 0;
	virtual bool is_cfun() const = 0;
	virtual bool is_macro() const = 0;

	virtual Tokid get_definition() const = 0;
	// Return an entry's identifying site
	Tokid get_site() const {
		if (is_defined())
			return get_definition();
		else
			return get_tokid();
	}

	// ctor; never call it if the call for t already exists
	Call(const string &s, const Token &t);
	virtual ~Call() {}
};

#endif // CALL_
