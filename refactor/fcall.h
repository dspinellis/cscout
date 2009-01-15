/*
 * (C) Copyright 2003-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Function call graph information
 *
 * $Id: fcall.h,v 1.16 2009/01/15 14:32:57 dds Exp $
 */

#ifndef FCALL_
#define FCALL_

#include "call.h"
#include "type.h"
#include "tokid.h"
#include "token.h"

/*
 * Function call information is always associated with Id objects
 * It is thus guaranteed to match the symbol table lookup operations
 */

// C function calling information
class FCall : public Call {
private:
	Tokid definition;		// Function's definition
	Type type;			// Function's type
	bool defined;			// True if the function has been defined
public:
	// Set the C function currently being parsed
	static void set_current_fun(const Type &t);
	static void set_current_fun(const Id *id);

	virtual Tokid get_definition() const { return definition; }
	virtual bool is_defined() const { return defined; }
	virtual bool is_declared() const { return true; }
	virtual bool is_file_scoped() const { return type.is_static(); }
	virtual bool is_cfun() const { return true; }
	virtual bool is_macro() const { return false; }
	virtual const string & entity_type_name() const {
		static string s("C function");
		return (s);
	}
	FCall(const Token& t, Type typ, const string &s);

	virtual ~FCall() {}
};

#endif // FCALL_
