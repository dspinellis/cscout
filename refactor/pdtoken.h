/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor directive token.
 * The getnext() method for these tokens performs preprocessor directives
 * on the lexical tokens.
 *
 * $Id: pdtoken.h,v 1.12 2001/09/01 05:33:56 dds Exp $
 */

#ifndef PDTOKEN_
#define PDTOKEN_

class Pdtoken;

typedef list<Pdtoken> listPdtoken;

typedef deque<Ptoken> dequePtoken;
typedef list<Ptoken> listPtoken;
typedef set<string> setstring;
typedef map<string, listPtoken> mapArgval;
typedef stack<bool> stackbool;

// A macro definition
class Macro {
public:
	Ptoken name_token;		// Name (used for unification)
	bool is_function;		// True if it is a function-macro
	dequePtoken formal_args;	// Formal arguments (names)
	dequePtoken value;		// Macro value
	inline friend bool operator ==(const Macro& a, const Macro& b);
	inline friend bool operator !=(const Macro& a, const Macro& b) { return !(a == b); };
};

typedef map<string, Macro> mapMacro;

enum e_if_type {if_plain, if_def, if_ndef, if_elif};

class Pdtoken: public Ptoken {
private:
	static mapMacro macros;			// Defined macros
	static listPtoken expand;		// Expanded input
	static bool at_bol;			// At beginning of line
	static stackbool iftaken;		// Taken #ifs
	static bool skipping;			// Skip tokens when true
	static int iflevel;			// Level of enclosing #ifs

	static void process_directive();	// Handle a cpp directive
	static void eat_to_eol();		// Consume input including \n
	static void process_include();		// Handle a #include
	static void process_define();		// Handle a #define
	static void process_undef();		// Handle a #undef
	static void process_if(enum e_if_type);	// Handle #if/def/ndef
	static void process_else();		// Handle a #define
	static void process_endif();		// Handle a #define
	static void process_line();		// Handle a #line
	static void process_error();		// Handle a #error
	static void process_pragma();		// Handle a #pragma
public:
	// Constructors
	Pdtoken(const Ptoken &pt) : Ptoken(pt) {};
	Pdtoken() {};
	void getnext();
	// Clear the defined macro table (when changing compilation unit)
	static void clear_macros() { macros.clear(); }
	friend listPtoken::iterator macro_replace(listPtoken& tokens, listPtoken::iterator pos, setstring tabu, bool get_more);
};

// True if two macro definitions are the same
inline bool
operator ==(const Macro& a, const Macro& b)
{
	return (a.is_function == b.is_function && 
		a.formal_args == b.formal_args &&
		a.value == b.value);
}

#endif // PDTOKEN
