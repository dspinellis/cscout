/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information. Macros.
 *
 * $Id: mcall.h,v 1.3 2004/07/25 11:47:35 dds Exp $
 */

#ifndef MCALL_
#define MCALL_

// Function-like macro calling information
class MCall : public Call {
private:
public:

	virtual bool is_defined() const { return true; }
	virtual bool is_declared() const { return false; }
	virtual Tokid get_definition() const { return tokid; }
	virtual const string & entity_type_name() const {
		static string s("function-like macro");
		return (s);
	}

	MCall(const Token& tok, const string &s) :
		Call(s, tok.get_parts_begin()->get_tokid())
	{}

	virtual ~MCall() {}
};

#endif // MCALL_
