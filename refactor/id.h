/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * An C identifier as returned by the lexical analyser of parsing phase.
 *
 * $Id: id.h,v 1.1 2001/09/04 13:27:54 dds Exp $
 */

#ifndef ID_
#define ID_

class Type;

class Id {
private:
	Token tok;
	Type *type;
	string name;
public:
	Type *get_type() const { return type; };
	const string& get_name() const { return name; };
	const Token& get_token() const { return tok; };
};

#endif // ID_
