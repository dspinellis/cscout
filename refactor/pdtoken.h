/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A preprocessor directive token.
 * The getnext() method for these tokens performs preprocessor directives
 * on the lexical tokens.
 *
 * Include synopsis:
 * #include <iostream>
 * #include <map>
 * #include <string>
 * #include <deque>
 * #include <stack>
 * #include <iterator>
 * #include <list>
 * #include <cassert>
 * 
 * #include "cpp.h"
 * #include "fileid.h"
 * #include "tokid.h"
 * #include "token.h"
 * #include "error.h"
 * #include "ptoken.h"
 * #include "fchar.h"
 * #include "ytab.h"
 * #include "pltoken.h"
 *
 * $Id: pdtoken.h,v 1.2 2001/08/21 08:35:16 dds Exp $
 */

#ifndef PDTOKEN_
#define PDTOKEN_

class Pdtoken;

typedef list<Pdtoken> listPdtoken;

typedef deque<Pltoken> dequePltoken;
typedef map<string, dequePltoken> mapArgval;

class Macro {
public:
	bool is_function;		// True if it is a function-macro
	dequePltoken formal_args;	// Formal arguments (names)
	dequePltoken value;		// Macro value
};

typedef map<string, Macro> mapMacro;

enum e_if_type {if_plain, if_def, if_ndef, if_elif};

class Pdtoken: public Ptoken {
private:
	static mapMacro macros;			// Defined macros
	static listPdtoken expand;		// Expanded input
	static bool at_bol;			// At beginning of line

	static void process_directive();	// Handle a cpp directive
	static void expand_macro(Pltoken m);	// Expand a macro
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
};

#endif // PDTOKEN
