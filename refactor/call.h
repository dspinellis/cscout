/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information
 *
 * $Id: call.h,v 1.3 2004/07/24 07:56:06 dds Exp $
 */

#ifndef CALL_
#define CALL_

class FCall;

/*
 * Generic call information
 * Can be macro or function.
 */
class Call {
private:

	// Container for storing all declared functions
	typedef set <Call *> fun_container;
	typedef map <Tokid, Call *> fun_map;

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
	Tokid tokid;

public:
	// The current function makes a call to f
	static void register_call(Call *f);
	// A call from from to to
	static void register_call(Call *from, Call *to);

	// Clear the visit flags for all functions
	static void clear_visit_flags();

	// Interface for iterating through all functions
	typedef fun_map::const_iterator const_fmap_iterator_type;
	static const_fmap_iterator_type fbegin() { return all.begin(); }
	static const_fmap_iterator_type fend() { return all.end(); }

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

	// Return a tokid for the given object
	Tokid get_tokid() const {return tokid; }

	// Return true if the function is defined
	virtual bool is_defined() const = 0;

	virtual Tokid get_definition() const = 0;
	// Return an entry's identifying site
	Tokid get_site() const {
		if (is_defined())
			return get_definition();
		else
			return get_tokid();
	}

	Call(const string &s, Tokid t);
	virtual ~Call() {}
};

#endif // CALL_
