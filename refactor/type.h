/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 *
 * $Id: type.h,v 1.5 2001/09/12 07:09:08 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

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

enum e_tagtype {tt_struct, tt_union, tt_enum};

class Id;

class Type_node {
	friend class Type;
private:
	int use;				// Use count
protected:
	Type_node() : use(1) {}

	virtual Type subscript() const;		// Arrays and pointers
	virtual Type deref() const;		// Arrays and pointers
	virtual Type call() const;		// Function
	virtual Type type() const;		// Typedef
	virtual Id member(const string& name) const;	// Structure and union
	virtual bool is_ptr() const { return false; }// True for ptr arithmetic types
	virtual bool is_typedef() const { return false; }// True for typedefs
	virtual bool is_valid() const { return true; }// False for undeclared
	virtual const string& get_name() const;	// True for identifiers
	virtual const Ctoken& get_token() const;// True for identifiers
};


// Basic type
class Tbasic: public Type_node {
private:
	enum e_btype type;
	enum e_sign sign;
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none) :
		type(t), sign(s) {}
	bool is_valid() const { return type != b_undeclared; }
};

/*
 * Handle class for representing types.
 * It encapsulates the type node memory management.
 * See Koening & Moo: Ruminations on C++ Addison-Wesley 1996, chapter 8
 */
class Type {
private:
	Type_node *p;
	Type(Type_node *n) : p(n) {}
public:
	Type() { p = new Tbasic(b_undeclared); }
	// Creation functions
	friend Type basic(enum e_btype t = b_abstract, enum e_sign s = s_none);
	friend Type array_of(Type t);
	friend Type pointer_to(Type t);
	friend Type function_returning(Type t);
	friend Type implict_function();
	friend Type typedef_for(Type t);
	friend Type enum_tag();
	friend Type struct_tag();
	friend Type union_tag();
	friend Type identifier(const Ctoken& c);

	// Manage use count of underlying Type_node
	Type(const Type& t) { p = t.p; ++p->use; }	// Copy
	~Type() { if (--p->use == 0) delete p; }
	Type& operator=(const Type& t);

	// Interface to the Type_node functionality
	Type subscript() const		{ return p->subscript(); }
	Type deref() const		{ return p->deref(); }
	Type call() const		{ return p->call(); }
	Type type() const		{ return p->type(); }
	bool is_ptr() const		{ return p->is_ptr(); }
	bool is_typedef() const		{ return p->is_typedef(); }
	bool is_valid() const		{ return p->is_valid(); }
	const string& get_name() const	{ return p->get_name(); }
	const Ctoken& get_token() const { return p->get_token(); }
	Id member(const string& name) const;
};


// Array of ...
class Tarray: public Type_node {
private:
	Type of;
public:
	Tarray(Type t) : of(t) {}
	Type subscript() const { return of; }
	bool is_ptr() { return true; }
};

// Pointer to ...
class Tptr: public Type_node {
private:
	Type to;
public:
	Tptr(Type t) : to(t) {}
	Type deref() const { return to; }
	bool is_ptr() { return true; }
};

// Function returning ...
class Tfunction: public Type_node {
private:
	Type returning;
public:
	Tfunction(Type t) : returning(t) {}
	Type call() const { return returning; }
};

// Implicit declaration of function(...) returning int
// This is the default for all undeclared identifiers
class Timplicit: public Type_node {
public:
	Type call() const { Type t = basic(b_int); return (t); }
};


// Typedef for ...
class Ttypedef: public Type_node {
private:
	Type for_type;
public:
	Ttypedef(Type t) : for_type(t) {}
	Type type() const { return for_type; }
	bool is_typedef() { return true; };// True for typedefs
};

// Tag for ..
class Ttag: public Type_node {
private:
	bool incomplete;
	enum e_tagtype type;
public:
	Ttag(enum e_tagtype e, bool i = true) :
		incomplete(i), type(e) {}
};

// Structure or Union
class Tsu: public Ttag {
private:
	map<string, Id> members;
public:
	Tsu(enum e_tagtype e, bool i = true) :
		Ttag(e, i) {}
	Id member(const string& name) const;
	void add_member(string& name, Id i);
};

// Identifier; not really a type, it is returned by the lexical analyser
class Tidentifier: public Type_node {
private:
	Ctoken t;
public:
	Tidentifier(const Ctoken& tok) : t(tok) {}
	const Ctoken& get_token() const { return t; }
	const string& get_name() const { return t.get_name(); }
};

/*
 * We can not use a union since its members have constructors and desctructors.
 * We thus define a structure with a single element to turn-on the yacc
 * type-checking mechanisms
 * Elements that have a type (which must be of type Type) are defined as <t>
 */
typedef struct {
	Type t;
} YYSTYPE;

#endif // TYPE_
