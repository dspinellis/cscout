/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor directive token.
 * The getnext() method for these tokens performs preprocessor directives
 * on the lexical tokens.
 *
 * $Id: pdtoken.h,v 1.31 2007/08/19 09:07:54 dds Exp $
 */

#ifndef PDTOKEN_
#define PDTOKEN_

class Pdtoken;

typedef list<Pdtoken> listPdtoken;

typedef deque<Ptoken> dequePtoken;
typedef list<Ptoken> PtokenSequence;
typedef set<string> setstring;
typedef map<string, PtokenSequence> mapArgval;
typedef stack<bool> stackbool;
typedef vector<string> vectorstring;


class Macro;

typedef map<string, Macro> mapMacro;

class Pdtoken: public Ptoken {
private:
	static mapMacro macros;			// Defined macros
	static mapMacroBody macro_body_tokens;	// Tokens and the macros they belong to
	static PtokenSequence expand;		// Expanded input

	static bool at_bol;			// At beginning of line
	static stackbool iftaken;		// Taken #ifs
	static int skiplevel;			// Level of enclosing #ifs

	static vectorstring include_path;	// Include file path

	static void process_directive();	// Handle a cpp directive
	static void eat_to_eol();		// Consume input including \n
	static void process_include(bool next);	// Handle a #include
	static void process_define();		// Handle a #define
	static void process_undef();		// Handle a #undef
	static void process_if();		// Handle #if
	static void process_ifdef(bool isndef);	// Handle #ifdef/ndef
	static void process_elif();		// Handle #elif
	static void process_else();		// Handle a #define
	static void process_endif();		// Handle a #define
	static void process_line();		// Handle a #line
	static void process_error(enum e_error_level e);
						// Handle a #error #warning
	static void process_pragma();		// Handle a #pragma
public:
	// Constructors
	Pdtoken(const Ptoken &pt) : Ptoken(pt) {};
	Pdtoken() {};
	void getnext();

	// Clear the defined macro table (when changing compilation unit)
	static void macros_clear() {
		macros.clear();
		macro_body_tokens.clear();
	}

	// Return the number of defined macros
	static mapMacro::size_type macros_size() {
		return macros.size();
	}

	// Find a macro given its name
	static mapMacro::const_iterator macros_find(const string& s) { return macros.find(s); }
	// Undefined macro returned by find
	static mapMacro::const_iterator macros_end() { return macros.end(); }
	// Given the result of macros_find return true of the macro is really defined
	static bool macro_is_defined(mapMacro::const_iterator mi) { return mi != macros.end() && (*mi).second.get_is_defined(); }
	// Add to the macros map an undefined macro
	static void create_undefined_macro(const Ptoken &name);
	// Add an element in the include path
	static void add_include(const string& s) { include_path.push_back(s); };
	// Clear the include path
	static void clear_include() { include_path.clear(); };
	// Called when we start processing a new file
	// or resume an old one.  We assume that files end in line
	// boundaries, even when they lack an explicit newline at their end
	static void file_switch() { at_bol = true; };
	// Return the macro where a given token resides
	static MCall *get_body_token_macro_mcall(Tokid t);
	// Return true if we are currently skipping due to conditional compilation
	static bool skipping() { return skiplevel != 0; }
};

ostream& operator<<(ostream& o,const dequePtoken &dp);

#endif // PDTOKEN
