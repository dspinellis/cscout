/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 *
 * $Id: type.h,v 1.10 2001/09/16 10:08:02 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

enum e_btype {
	b_abstract,		// Abstract declaration target, to be filled-in
	b_void, b_char, b_short, b_int, b_long, b_float, b_double, b_ldouble,
	b_undeclared,		// Undeclared object
	b_llong			// long long
};

enum e_sign {
	s_none,			// Neither signed, nor unsigned
	s_signed,
	s_unsigned
};

enum e_storage_class {
	c_unspecified,
	c_typedef,
	c_extern,
	c_static,
	c_auto,
	c_register
};


enum e_tagtype {tt_struct, tt_union, tt_enum};

class Id;
class Tbasic;

class Type_node {
	friend class Type;
private:
	int use;				// Use count
	// This is also the place to store type qualifiers, because the can be
	// applied to any type.  Furtunatelly we can afford to ignore them.
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
	virtual bool is_basic() const { return false; }// False for undeclared
	virtual bool is_abstract() const { return false; }	// True for abstract types
	virtual const string& get_name() const;	// True for identifiers
	virtual const Ctoken& get_token() const;// True for identifiers
	virtual void set_abstract(Type t);	// Set abstract basic type to t
public:
	// For merging
	virtual Type merge(Tbasic *b);
	virtual Tbasic *tobasic() { return NULL; }
	virtual void print(ostream &o) const = 0;
};


// Basic type
class Tbasic: public Type_node {
private:
	enum e_btype type;
	enum e_sign sign;
	enum e_storage_class sclass;
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none,
		enum e_storage_class sc = c_unspecified) :
		type(t), sign(s), sclass(sc) {}
	bool is_valid() const { return type != b_undeclared; }
	bool is_abstract() const { return type == b_abstract; }
	bool is_typedef() const { return sclass != c_typedef; }
	bool is_basic() const { return true; }// False for undeclared
	void print(ostream &o) const;
	Type merge(Tbasic *b);
	Tbasic *tobasic() { return this; }
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
	friend Type basic(enum e_btype t = b_abstract, enum e_sign s = s_none,
			  enum e_storage_class sc = c_unspecified);
	friend Type array_of(Type t);
	friend Type pointer_to(Type t);
	friend Type function_returning(Type t);
	friend Type implict_function();
	friend Type typedef_for(Type t);
	friend Type enum_tag();
	friend Type struct_tag();
	friend Type union_tag();
	friend Type label();
	friend Type identifier(const Ctoken& c);
	// To print
	friend ostream& operator<<(ostream& o,const Type &t) { t.p->print(o); }

	// Manage use count of underlying Type_node
	Type(const Type& t) { p = t.p; ++p->use; }	// Copy
	~Type() { if (--p->use == 0) delete p; }
	Type& operator=(const Type& t);

	// Interface to the Type_node functionality
	Type subscript() const		{ return p->subscript(); }
	Type deref() const		{ return p->deref(); }
	Type call() const		{ return p->call(); }
	Type type() const		{ return p->type(); }
	void set_abstract(Type t)	{ return p->set_abstract(t); }
	bool is_ptr() const		{ return p->is_ptr(); }
	bool is_typedef() const		{ return p->is_typedef(); }
	bool is_valid() const		{ return p->is_valid(); }
	bool is_basic() const		{ return p->is_basic(); }
	bool is_abstract() const	{ return p->is_abstract(); }
	const string& get_name() const	{ return p->get_name(); }
	const Ctoken& get_token() const { return p->get_token(); }
	Id member(const string& name) const;
	friend Type merge(Type a, Type b) { return a.p->merge(b.p->tobasic()); }
};


// Array of ...
class Tarray: public Type_node {
private:
	Type of;
public:
	Tarray(Type t) : of(t) {}
	Type subscript() const { return of; }
	bool is_ptr() { return true; }
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Pointer to ...
class Tpointer: public Type_node {
private:
	Type to;
public:
	Tpointer(Type t) : to(t) {}
	Type deref() const { return to; }
	bool is_ptr() { return true; }
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Function returning ...
class Tfunction: public Type_node {
private:
	Type returning;
public:
	Tfunction(Type t) : returning(t) {}
	Type call() const { return returning; }
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Typedef for ...
class Ttypedef: public Type_node {
private:
	Type for_type;
public:
	Ttypedef(Type t) : for_type(t) {}
	Type type() const { return for_type; }
	bool is_typedef() { return true; };// True for typedefs
	void print(ostream &o) const;
};

// Tag for ..
class Ttag: public Type_node {
private:
	bool incomplete;
	enum e_tagtype type;
public:
	Ttag(enum e_tagtype e, bool i = true) :
		incomplete(i), type(e) {}
	void print(ostream &o) const;
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
	void print(ostream &o) const;
};

// Identifier; not really a type, it is returned by the lexical analyser
// It is also the type used to represent undeclared identifiers
class Tidentifier: public Type_node {
private:
	Ctoken t;
	Type of;			// Identifying a given type
public:
	Tidentifier(const Ctoken& tok) : t(tok), of(basic()) {}
	const Ctoken& get_token() const { return t; }
	const string& get_name() const { return t.get_name(); }
	Type call() const;			// Function (undeclared)
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Goto label
class Tlabel: public Type_node {
public:
	Tlabel() {}
	void print(ostream &o) const;
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
