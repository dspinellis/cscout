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
 * $Id: pdtoken.h,v 1.1 2001/08/20 21:09:58 dds Exp $
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


class Pdtoken: public Ptoken {
private:
	static mapMacro macros;			// Defined macros
	static listPdtoken expand;		// Expanded input
	static bool at_bol;			// At beginning of line

	void process_directive();		// Handle a cpp directive
	void expand_macro(Pltoken m);		// Expand a macro
public:
	// Constructors
	Pdtoken(const Ptoken &pt) : Ptoken(pt) {};
	Pdtoken() {};
	void getnext();
	// Clear the defined macro table (when changing compilation unit)
	static void clear_macros() { macros.clear(); }
};

#endif // PDTOKEN
