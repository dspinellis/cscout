/*
 * (C) Copyright 2004-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Function call graph information. Macros.
 *
 * $Id: mcall.h,v 1.9 2009/01/15 14:32:57 dds Exp $
 */

#ifndef MCALL_
#define MCALL_

#include "call.h"
#include "macro.h"

// Function-like macro calling information
class MCall : public Call {
private:
public:
	// Set the macro currently being processed
	static void set_current_fun(Macro &m);

	virtual bool is_defined() const { return true; }
	virtual bool is_declared() const { return false; }
	virtual bool is_file_scoped() const { return true; }
	virtual Tokid get_definition() const { return get_tokid(); }
	virtual bool is_cfun() const { return false; }
	virtual bool is_macro() const { return true; }
	virtual const string & entity_type_name() const;

	MCall(const Token& tok, const string &s) :
		Call(s, tok)
	{}

	virtual ~MCall() {}
};

#endif // MCALL_
