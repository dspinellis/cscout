/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information
 *
 * $Id: call.h,v 1.25 2008/11/16 12:31:05 dds Exp $
 */

#ifndef CALL_
#define CALL_

#include "funmetrics.h"

class FCall;
class Sql;
class Id;
class Ctoken;

/*
 * Generic call information of a called/calling entity.
 * The calls can be to/from macros or functions.
 * We identify four types of calls:
 * 1. Macro calls macro
 *	Function like macro-expansion while expanding a macro
 * 2. Macro calls function
 *	parse.y type.cpp call register_call(id->get_fcall())
 *	Function call with the identifier token being part of a
 *	(function-like) macro body
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
	/*
	 * When processing the program a Call * is stored with each Id
	 * This allows accurate lookup of calls within a linkage unit.
	 * However, once a linkage unit goes out of scope, we need further
	 * help to reunite functions from varying projects and also locate
	 * function declarations when processing the source code.  A function
	 * can be identified well enough by using the Tokid of its declaration.
	 * However, the same Tokid can through token pasting be used for
	 * declaring multiple functions.  Therefore we use a multimap, and
	 * as a second step when we lookup a function we compare the
	 * corresponding tokens.
	 */
	typedef multimap <Tokid, Call *> fun_map;

	string name;			// Function's name
	fun_container call;		// Functions this function calls
	fun_container caller;		// Functions that call this function
	bool visited;			// For calculating transitive closures
	bool printed;			// For printing a graph's nodes
	FcharContext begin, end;	// Span of definition
	FunMetrics m;			// Metrics for this function
	int curr_stmt_nesting;		// Current level of nesting
	static int macro_nesting;	// Level of nesting through macro tokens

	void add_call(Call *f) { call.insert(f); }
	void add_caller(Call *f) { caller.insert(f); }

protected:
	static fun_map all;		// Set of all functions
	static Call *current_fun;	// Function currently being parsed
	static stack<Call *> nesting;	// Nested function definitions

	/*
	 * A token (almost) uniquely identifying the call entity.
	 * (See fun_map comment for more details on the "almost".)
	 * Examples:
	 * Function's first declaration
	 * (could also be reference if implicitly declared)
	 * Macro's definition
	 */
	Token token;

public:
	// Called when outside a function / macro body scope
	static void unset_current_fun();
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
	static void clear_print_flags();

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

	// Dump the data in SQL format
#ifdef COMMERCIAL
	static void dumpSql(Sql *db, ostream &of);
#endif /* COMMERCIAL */

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
	void set_printed() { printed = true; }
	bool is_printed() const { return printed; }

	// Mark the function's span
	void mark_begin();
	FcharContext get_begin() const { return begin; }
	// Mark the function's span and add it to the corresponding file
	void mark_end();
	FcharContext get_end() const { return end; }
	// Return true if the span represents a file region
	bool is_span_valid() const;
	// Return a reference to the Metrics class
	FunMetrics &metrics() { return m; }
	// Return a reference to the Metrics class
	const FunMetrics &const_metrics() const { return m; }

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

	// Set number of arguments
	static inline void set_num_args(int n) {
		if (current_fun && !current_fun->m.is_processed())
			current_fun->m.set_metric(FunMetrics::em_nparam, n);
	}

	// Process a token destined for preprocessing
	static inline void process_token(const Pltoken &t) {
		if (current_fun && !current_fun->m.is_processed())
			current_fun->m.process_token(t);
	}

	// Call the specified metrics function for the current function
	static inline void call_metrics(void (Metrics::*fun)()) {
		if (current_fun)
			(current_fun->m.*fun)();
	}

	// The following three methods must always be called as a group
	// See if we have started nesting through macro-expanded tokens
	static void check_macro_nesting(const Ctoken &t);

	// Increase the current function's level of nesting
	static inline void increase_nesting() {
		if (current_fun && !current_fun->m.is_processed() &&
		    !macro_nesting)
			current_fun->m.update_nesting(++(current_fun->curr_stmt_nesting));
	}

	// Decrease the current function's level of nesting
	static inline void decrease_nesting() {
		if (!current_fun || current_fun->m.is_processed())
			return;
		if (macro_nesting)
			macro_nesting--;
		else
			current_fun->curr_stmt_nesting--;
	}

	// ctor; never call it if the call for t already exists
	Call(const string &s, const Token &t);
	virtual ~Call() {}
};

#endif // CALL_
