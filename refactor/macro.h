/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor macro definition.
 *
 * $Id: macro.h,v 1.6 2003/07/29 18:34:52 dds Exp $
 */

#ifndef MACRO_
#define MACRO_

class Pdtoken;

typedef deque<Ptoken> dequePtoken;
typedef list<Ptoken> listPtoken;
typedef set<string> setstring;
typedef map<string, listPtoken> mapArgval;
typedef stack<bool> stackbool;

// A macro definition
class Macro {
private:
	Ptoken name_token;		// Name (used for unification)
	bool is_function;		// True if it is a function-macro
	bool is_vararg;			// True if the function has variable # of arguments (gcc)
	bool is_defined;		// True if the macro has been defined
					// Can be false through ifdef/ifndef/defined()
	dequePtoken formal_args;	// Formal arguments (names)
	dequePtoken value;		// Macro value
public:
	Macro() {}
	Macro( const Ptoken& name, bool id) :
		name_token(name), is_defined(id) {}
	// Accessor functions
	const Ptoken& get_name_token() const {return name_token; };
	void set_is_function(bool v) { is_function = v; };
	void set_is_vararg(bool v) { is_vararg = v; };
	bool get_is_defined() const { return is_defined; };
	void form_args_push_back(Ptoken& t) { formal_args.push_back(t); };
	void value_push_back(Ptoken& t) { value.push_back(t); };

	// Remove trailing whitespace
	void value_rtrim();

	// Compare per ANSI C
	friend bool operator ==(const Macro& a, const Macro& b);
	inline friend bool operator !=(const Macro& a, const Macro& b) { return !(a == b); };
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Macro &m);

	friend listPtoken::iterator macro_replace(listPtoken& tokens, listPtoken::iterator pos, setstring tabu, bool get_more, listPtoken::iterator& valid_iterator);
};

void macro_replace_all(listPtoken& tokens, listPtoken::iterator end, setstring& tabu, bool get_more);


#endif // MACRO
