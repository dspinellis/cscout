/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 *
 * $Id: type.h,v 1.4 2001/09/10 13:48:54 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

class Type {
public:
	virtual Type *subscript();		// Arrays and pointers
	virtual Type *deref();			// Arrays and pointers
	virtual Type *call();			// Function
	virtual Type *type();			// Typedef
	virtual Id *member(const string& name);	// Structure and union
	virtual bool is_ptr() { return false; };// True for ptr arithmetic types
	virtual bool is_typedef() { return false; };// True for typedefs
};

enum e_btype {
	b_abstract,		// Abstract declaration target, to be filled-in
	b_void, b_char, b_short, b_int, b_long, b_float, b_double, b_ldouble,
	b_undeclared,		// Undeclared object
	b_type			// Typedef
};

enum e_sign {
	s_none,			// Neither signed, nor unsigned
	s_signed,
	s_unsigned
};

// Basic type
class Tbasic: public Type {
private:
	enum e_btype type;
	enum e_sign sign;
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none) :
		type(t), sign(s) {};
};

// Array of ...
class Tarray: public Type {
private:
	Type *of;
public:
	Tarray(Type *t) { of = t; };
	~Tarray() { delete of; };
	Type *subscript() const { return of; };
	bool is_ptr() { return true; };
};

// Pointer to ...
class Tptr: public Type {
private:
	Type *to;
public:
	Tptr(Type *t) { to = t; };
	~Tptr() { delete to; };
	Type *deref() const { return to; };
	bool is_ptr() { return true; };
};

// Function returning ...
class Tfun: public Type {
private:
	Type *returning;
public:
	Tfun(Type *t) { returning = t; };
	~Tfun() { delete returning; };
	Type *call() const { return returning; };
};

// Implicit declaration of function(...) returning int
// This is the default for all undeclared identifiers
class Timplicit: public Type {
public:
	Type *call() const { new Tbasic(b_int); };
};


// Typedef for ...
class Ttypedef: public Type {
private:
	Type *for_type;
public:
	Ttypedef(Type *t) { for_type = t; };
	~Ttypedef() { delete for_type; };
	Type *type() const { return for_type; };
	bool is_typedef() { return true; };// True for typedefs
};

enum e_tagtype {tt_struct, tt_union, tt_enum};

// Tag for ..
class Ttag: public Type {
private:
	bool incomplete;
	enum e_tagtype type;
public:
	Ttag(enum e_tagtype e, bool i = true) :
		incomplete(i), type(e) {};
};

// Structure or Union
class Tsu: public Ttag {
private:
	map<string, Id *> members;
public:
	Tsu(enum e_tagtype e, bool i = true) :
		Ttag(e, i) {};
	~Tsu();
	Id *member(const string& name) const;
	Type *add_member(string& name, Id *i) {members[name] = i; };
};

#endif // TYPE_
