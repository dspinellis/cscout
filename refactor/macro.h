/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A preprocessor macro definition.
 *
 * $Id: macro.h,v 1.20 2009/01/15 14:32:57 dds Exp $
 */

#ifndef MACRO_
#define MACRO_

#include <deque>
#include <list>
#include <map>
#include <set>
#include <stack>

using namespace std;

#include "ptoken.h"

class Pdtoken;
class Macro;
class MCall;

typedef deque<Ptoken> dequePtoken;
typedef list<Ptoken> PtokenSequence;
typedef set<string> setstring;
typedef map<string, PtokenSequence> mapArgval;
typedef stack<bool> stackbool;
/*
 * We map to MCall * instead of Macro *, because Macro are stored
 * in a container and tend to move around.
 */
typedef map<Tokid, MCall *> mapMacroBody;

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
	MCall *mcall;			// Function call info
public:
	Macro( const Ptoken& name, bool id, bool is_function);
	// Accessor functions
	const Ptoken& get_name_token() const {return name_token; };
	void set_is_function(bool v) { is_function = v; };
	void set_is_vararg(bool v) { is_vararg = v; };
	bool get_is_defined() const { return is_defined; };
	bool get_is_vararg() const { return is_vararg; };
	// Return the number of formal arguments
	int get_num_args() const { return formal_args.size(); }
	void form_args_push_back(Ptoken& t) { formal_args.push_back(t); };
	void value_push_back(Ptoken& t) { value.push_back(t); };
	MCall *get_mcall() const { return mcall; }

	// Remove trailing whitespace
	void value_rtrim();

	// Update the map to include the macro's body refering to the macro
	void register_macro_body(mapMacroBody &map) const;

	// Compare per ANSI C
	friend bool operator ==(const Macro& a, const Macro& b);
	inline friend bool operator !=(const Macro& a, const Macro& b) { return !(a == b); };
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Macro &m);

	friend PtokenSequence macro_expand(PtokenSequence ts, bool get_more, bool skip_defined, const Macro *caller);
};

PtokenSequence macro_expand(PtokenSequence ts, bool get_more, bool skip_defined, const Macro *caller = NULL);
#endif // MACRO
