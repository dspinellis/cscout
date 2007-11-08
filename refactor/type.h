/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 * See also type2.h for derived classes depending on Stab
 *
 * $Id: type.h,v 1.44 2007/11/08 17:48:05 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

#include "ctconst.h"

enum e_btype {
	b_abstract,		// Abstract declaration target, to be filled-in
	b_void, b_char, b_short, b_int, b_long, b_float, b_double, b_ldouble,
	b_bool,
	b_padbit,
	b_undeclared,		// Undeclared object
	b_llong,		// long long
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
	c_register,
	c_enum
};

enum e_qualifier {
	q_none = 	0x00,
	q_const =	0x01,
	q_volatile = 	0x02,
	q_unused =	0x04,	// gcc __attribute__((unused))
	q_restrict = 	0x08,
};


class Id;
class Tbasic;
class Type;
class Stab;
class Tqualifier;

class Type_node {
	friend class Type;
private:
#ifdef NODE_USE_PROFILE
	static int count;
#endif
	int use;				// Use count
	// Do not allow copy and assignment; it has to be performed around Type
	Type_node(const Type_node &);
	Type_node& operator=(const Type_node &);
protected:
	Type_node() : use(1) {
#ifdef NODE_USE_PROFILE
		count++;
#endif
	}

	virtual ~Type_node() {
#ifdef NODE_USE_PROFILE
		count--;
#endif
	}
	virtual Type subscript() const;		// Arrays and pointers
	virtual Type deref() const;		// Arrays and pointers
	virtual Type call() const;		// Function
	virtual Type type(Type dftl) const;	// Identifier
	virtual Type clone() const;		// Deep copy
	virtual Id const* member(const string& name) const;	// Structure and union
	virtual Type member(int n);	// For > 0 on structure, union, array
	virtual bool is_abstract() const { return false; }	// True for abstract types
	virtual bool is_array() const { return false; }	// True for arrays
	virtual bool is_basic() const { return false; }// False for undeclared
	virtual bool is_function() const { return false; }	// True for functions
	virtual bool is_identifier() const { return false; }// True only for identifiers
	virtual bool is_incomplete() const { return false; }	// True incomplete struct/union
	virtual bool is_padbit() const { return false; }// True for pad bit field
	virtual bool is_char() const { return false; }// True for pad bit field
	virtual bool is_ptr() const { return false; }// True for ptr arithmetic types
	virtual bool is_su() const { return false; }// True for struct/union
	virtual bool is_valid() const { return true; }// False for undeclared
	virtual bool is_void() const { return false; }// True only for the void basic type
	virtual bool qualified_const() const { return false; }// True for constructs containing the const attribute
	virtual bool qualified_restrict() const { return false; }// True for constructs containing the restrict attribute
	virtual bool qualified_unused() const { return false; }// True for constructs containing the unused attribute
	virtual bool qualified_volatile() const { return false; }// True for constructs containing the volatile attribute
	virtual const string& get_name() const;	// True for identifiers
	virtual const Ctoken& get_token() const;// True for identifiers
	virtual void set_abstract(Type t);	// Set abstract basic type to t
	virtual void set_storage_class(Type t);	// Set typedef's underlying storage class to t
	virtual void clear_storage_class();	// Clear underlying storage class
	virtual enum e_storage_class get_storage_class() const;// Return the declaration's storage class
	virtual int get_qualifiers() const;// Return the declaration's qualifiers
	virtual int get_nparam() const;	// Return the number of parameters
	virtual void add_param();	// Add another parameter to the list
	virtual CTConst get_value() const {return CTConst(); }	// Return the value of a compile-time constant
	// Return the number of elements this type can hold (array, structure, union)
	virtual CTConst get_nelem() const { return CTConst(1); }
	virtual void set_union(bool v);		// True for union false for struct
	virtual void set_value(CTConst v) {}	// Set the value of a compile-time constant
	virtual void add_qualifiers(Type t);		// Set our qualifiers to t
	virtual void add_member(const Token &tok, const Type &typ);
	virtual Type get_default_specifier() const;
	virtual void merge_with(Type t);
	virtual const Stab& get_members_by_name() const;
	virtual const vector <Id>& get_members_by_ordinal() const;

	bool is_typedef() const { return get_storage_class() == c_typedef; }// True for typedefs
	bool is_static() const { return get_storage_class() == c_static; }// True for static
public:
	// For merging
	virtual Type merge(Tbasic *b);
	virtual Tbasic *tobasic();
	virtual void print(ostream &o) const = 0;
#ifdef NODE_USE_PROFILE
	static int get_count();
#endif
};

// Used by types with a storage class: Tbasic, Tsu, Tenum, Tincomplete, Tptr, Tarray
class Tstorage {
private:
	enum e_storage_class sclass;
public:
	Tstorage (enum e_storage_class sc) : sclass(sc) {}
	Tstorage() : sclass(c_unspecified) {}
	enum e_storage_class get_storage_class() const {return sclass; }
	void set_storage_class(Type t);
	void clear_storage_class();
	void print(ostream &o) const;
};

// A type node with a qualifier
class QType_node : public Type_node {
public:
	typedef int qualifiers_t;
protected:
	qualifiers_t qualifiers;
public:
	QType_node (qualifiers_t q = q_none) : qualifiers(q) {}
	virtual bool qualified_const() const { return (qualifiers & q_const); }
	virtual bool qualified_restrict() const { return (qualifiers & q_restrict); }
	virtual bool qualified_unused() const { return (qualifiers & q_unused); }
	virtual bool qualified_volatile() const { return (qualifiers & q_volatile); }
	void set_qualifiers(Type t);
	void set_qualifiers(qualifiers_t q) { qualifiers = q; }
	virtual qualifiers_t get_qualifiers() const { return qualifiers; }
	void add_qualifier(enum e_qualifier q) { qualifiers |= q; }
	virtual void add_qualifiers(qualifiers_t q) { qualifiers |= q; }
	inline void add_qualifiers(Type t);
	void print(ostream &o) const;
};

typedef QType_node::qualifiers_t qualifiers_t;

// Basic type
class Tbasic: public QType_node {
private:
	enum e_btype type;	// Basic type
	enum e_sign sign;	// Sign
	Tstorage sclass;	// Storage class
	CTConst value;		// Value of a compile-time constant
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none,
		enum e_storage_class sc = c_unspecified, qualifiers_t q = q_none) :
		QType_node(q), type(t), sign(s), sclass(sc) {}
	Type clone() const;
	bool is_valid() const { return type != b_undeclared && type != b_padbit; }
	bool is_abstract() const { return type == b_abstract; }
	bool is_basic() const { return true; }// False for undeclared
	bool is_void() const { return type == b_void; }
	bool is_padbit() const { return type == b_padbit; }
	bool is_char() const { return type == b_char; }
	void print(ostream &o) const;
	Type merge(Tbasic *b);
	Tbasic *tobasic() { return this; }
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	inline void set_storage_class(Type t);
	inline void clear_storage_class();
	void set_abstract(Type t);		//For padbits
	CTConst get_value() const {return value; }
	void set_value(CTConst v) { value = v; }
};

/*
 * Handle class for representing types.
 * It encapsulates the type node memory management.
 * See Koening & Moo: Ruminations on C++ Addison-Wesley 1996, chapter 8
 */
class Type {
private:
	Type_node *p;
public:
	Type(Type_node *n) : p(n) {}
	Type() { p = new Tbasic(b_undeclared); }
	// Creation functions
	friend Type basic(enum e_btype t, enum e_sign s,
			  enum e_storage_class sc, qualifiers_t);
	friend Type array_of(Type t, CTConst nelem = CTConst());
	friend Type pointer_to(Type t);
	friend Type function_returning(Type t, int n);
	friend Type implict_function();
	friend Type enum_tag();
	friend Type struct_union(const Token &tok, const Type &typ, const Type &spec);
	friend Type struct_union(const Type &spec);
	friend Type struct_union();
	friend Type label();
	friend Type plist(int n);
	friend Type identifier(const Ctoken& c);
	friend Type incomplete(const Ctoken& c, int l);
	// To print
	friend ostream& operator<<(ostream& o,const Type &t) { t.p->print(o); return o; }

	// Add the declaration of an identifier to the symbol table
	void declare();

	// Manage use count of underlying Type_node
	Type(const Type& t) { p = t.p; ++p->use; }	// Copy
	~Type() { if (--p->use == 0) delete p; }
	Type& operator=(const Type& t);

	// Interface to the Type_node functionality
	Type clone() const 		{ return p->clone(); }
	Type subscript() const		{ return p->subscript(); }
	Type deref() const		{ return p->deref(); }
	Type call() const		{ return p->call(); }
	Type type() const		{ return p->type(*this); }
	void set_abstract(Type t)	{ return p->set_abstract(t); }
	void set_storage_class(Type t)	{ return p->set_storage_class(t); }
	void clear_storage_class()	{ return p->clear_storage_class(); }
	void add_param()		{ p->add_param(); }
	int get_nparam() const		{ return p->get_nparam(); }
	CTConst get_value() const	{ return p->get_value(); }
	CTConst get_nelem() const 	{ return p->get_nelem(); }
	void set_union(bool v)		{ p->set_union(v); }
	void set_value(CTConst v)	{ p->set_value(v); }
	bool is_abstract() const	{ return p->is_abstract(); }
	bool is_array() const		{ return p->is_array(); }
	bool is_basic() const		{ return p->is_basic(); }
	bool is_function() const	{ return p->is_function(); }
	bool is_identifier() const	{ return p->is_identifier(); }
	bool is_incomplete() const	{ return p->is_incomplete(); }
	bool is_padbit() const		{ return p->is_padbit(); }
	bool is_char() const		{ return p->is_char(); }
	bool is_ptr() const		{ return p->is_ptr(); }
	bool is_static() const		{ return p->is_static(); }
	bool is_su() const		{ return p->is_su(); }
	bool is_typedef() const		{ return p->is_typedef(); }
	bool is_valid() const		{ return p->is_valid(); }
	bool is_void() const		{ return p->is_void(); }
	bool qualified_const() const	{ return p->qualified_const(); }
	bool qualified_restrict() const	{ return p->qualified_restrict(); }
	bool qualified_unused() const	{ return p->qualified_unused(); }
	bool qualified_volatile() const	{ return p->qualified_volatile(); }
	void add_qualifiers(Type t)	{ return p->add_qualifiers(t); }
	qualifiers_t get_qualifiers() const { return p->get_qualifiers(); }
	const string& get_name() const	{ return p->get_name(); }
	const Ctoken& get_token() const { return p->get_token(); }
	enum e_storage_class get_storage_class() const
					{return p->get_storage_class(); }
	Type get_default_specifier() const
					{ return p->get_default_specifier(); }
	void add_member(const Token &tok, const Type &typ)
					{ p->add_member(tok, typ); }
	void merge_with(Type t) { p->merge_with(t) ; }
	const Stab& get_members_by_name() const	{ return p->get_members_by_name(); }
	const vector <Id>& get_members_by_ordinal() const	{ return p->get_members_by_ordinal(); }
	Id const* member(const string& name) const	// Structure and union
					{ return p->member(name); }
	Type member(int n) 		// Structure, union, array
					{ return p->member(n); }
	friend Type merge(Type a, Type b);
};

Type identifier(const Ctoken& c);
Type basic(enum e_btype t = b_abstract, enum e_sign s = s_none,
  enum e_storage_class sc = c_unspecified, qualifiers_t = q_none);
Type enum_tag();
Type struct_union();
Type incomplete(const Ctoken& c, int l);
Type label();
Type plist(int n);


inline void QType_node::add_qualifiers(Type t) {qualifiers |= t.get_qualifiers(); }
inline void Tbasic::set_storage_class(Type t) { sclass.set_storage_class(t); }
inline void Tbasic::clear_storage_class() { sclass.clear_storage_class(); }

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
