/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information. Macros.
 *
 * $Id: mcall.h,v 1.7 2007/08/12 07:22:01 dds Exp $
 */

#ifndef MCALL_
#define MCALL_

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
