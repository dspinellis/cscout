/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor macro definition.
 * The getnext() method for these tokens performs preprocessor directives
 * on the lexical tokens.
 *
 * $Id: macro.h,v 1.1 2001/09/02 15:02:00 dds Exp $
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
	dequePtoken formal_args;	// Formal arguments (names)
	dequePtoken value;		// Macro value
public:
	// Accessor functions
	const Ptoken& get_name_token() const {return name_token; };
	void set_name_token(Ptoken& t) { name_token = t; };
	void set_is_function(bool v) { is_function = v; };
	void form_args_push_back(Ptoken& t) { formal_args.push_back(t); };
	void value_push_back(Ptoken& t) { value.push_back(t); };

	// Remove trailing whitespace
	void value_rtrim();

	// Compare per ANSI C
	friend bool operator ==(const Macro& a, const Macro& b);
	inline friend bool operator !=(const Macro& a, const Macro& b) { return !(a == b); };

	friend listPtoken::iterator macro_replace(listPtoken& tokens, listPtoken::iterator pos, setstring tabu, bool get_more);
};

void macro_replace_all(listPtoken& tokens, listPtoken::iterator end, setstring& tabu, bool get_more);


#endif // MACRO
