/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information. Macros.
 *
 * $Id: mcall.h,v 1.6 2004/08/07 21:49:01 dds Exp $
 */

#ifndef MCALL_
#define MCALL_

// Function-like macro calling information
class MCall : public Call {
private:
public:

	virtual bool is_defined() const { return true; }
	virtual bool is_declared() const { return false; }
	virtual bool is_file_scoped() const { return true; }
	virtual Tokid get_definition() const { return get_tokid(); }
	virtual bool is_cfun() const { return false; }
	virtual bool is_macro() const { return true; }
	virtual const string & entity_type_name() const {
		static string s("function-like macro");
		return (s);
	}

	MCall(const Token& tok, const string &s) :
		Call(s, tok)
	{}

	virtual ~MCall() {}
};

#endif // MCALL_
