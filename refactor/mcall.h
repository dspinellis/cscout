/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * Function call graph information. Macros.
 *
 * $Id: mcall.h,v 1.1 2004/07/24 06:58:22 dds Exp $
 */

#ifndef MCALL_
#define MCALL_

// Function-like macro calling information
class MCall : public Call {
private:
public:

	virtual Tokid get_tokid() const { return definition; }
	virtual bool is_defined() const { return true; }

	MCall(const Token& tok, const string &s) :
		Call(s, tok.get_parts_begin()->get_tokid())
	{}

	virtual ~MCall() {}
};

#endif // MCALL_
