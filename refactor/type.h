/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 * See also type2.h for derived classes depending on Stab
 *
 * $Id: type.h,v 1.28 2003/08/17 17:00:27 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

enum e_btype {
	b_abstract,		// Abstract declaration target, to be filled-in
	b_void, b_char, b_short, b_int, b_long, b_float, b_double, b_ldouble,
	b_padbit,
	b_undeclared,		// Undeclared object
	b_llong,		// long long
	b_unused_attr		// Specified using __attribute__(unused)
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


class Id;
class Tbasic;
class Type;
class Stab;

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
	// This is also the place to store type qualifiers, because the can be
	// applied to any type.  Furtunatelly we can afford to ignore them.
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
	virtual Type type() const;		// Identifier
	virtual Type clone() const;		// Deep copy
	virtual Id const* member(const string& name) const;	// Structure and union
	virtual Id const* member(unsigned n) const;	// Structure and union
	virtual bool is_ptr() const { return false; }// True for ptr arithmetic types
	virtual bool is_su() const { return false; }// True for struct/union
	virtual bool is_valid() const { return true; }// False for undeclared
	virtual bool is_basic() const { return false; }// False for undeclared
	virtual bool is_padbit() const { return false; }// True for pad bit field
	virtual bool is_unused_attr() const { return false; }// True for constructs containing the unused attribute
	virtual bool is_abstract() const { return false; }	// True for abstract types
	virtual bool is_incomplete() const { return false; }	// True incomplete struct/union
	virtual bool is_array() const { return false; }	// True for arrays
	virtual bool is_function() const { return false; }	// True for functions
	virtual const string& get_name() const;	// True for identifiers
	virtual const Ctoken& get_token() const;// True for identifiers
	virtual void set_abstract(Type t);	// Set abstract basic type to t
	virtual void set_storage_class(Type t);	// Set typedef's underlying storage class to t
	virtual enum e_storage_class get_storage_class() const;// Return the declaration's storage class
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

// Used by types with a storage class: Tbasic and Tsu
class Tstorage {
private:
	enum e_storage_class sclass;
public:
	Tstorage (enum e_storage_class sc) : sclass(sc) {}
	Tstorage() { sclass = c_unspecified; }
	enum e_storage_class get_storage_class() const {return sclass; }
	void set_storage_class(Type t);
	void print(ostream &o) const;
};


// Basic type
class Tbasic: public Type_node {
private:
	enum e_btype type;
	enum e_sign sign;
	Tstorage sclass;
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none,
		enum e_storage_class sc = c_unspecified) :
		type(t), sign(s), sclass(sc) {}
	Type clone() const;
	bool is_valid() const { return type != b_undeclared && type != b_padbit; }
	bool is_abstract() const { return type == b_abstract; }
	bool is_basic() const { return true; }// False for undeclared
	bool is_padbit() const { return type == b_padbit; }
	bool is_unused_attr() const { return type == b_unused_attr; }
	void print(ostream &o) const;
	Type merge(Tbasic *b);
	Tbasic *tobasic() { return this; }
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	inline void set_storage_class(Type t);
	void set_abstract(Type t);		//For padbits
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
	friend Type basic(enum e_btype t = b_abstract, enum e_sign s = s_none,
			  enum e_storage_class sc = c_unspecified);
	friend Type array_of(Type t);
	friend Type pointer_to(Type t);
	friend Type function_returning(Type t);
	friend Type implict_function();
	friend Type enum_tag();
	friend Type struct_union(const Token &tok, const Type &typ, const Type &spec);
	friend Type struct_union(const Type &spec);
	friend Type struct_union();
	friend Type label();
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
	Type type() const		{ return p->type(); }
	void set_abstract(Type t)	{ return p->set_abstract(t); }
	void set_storage_class(Type t)	{ return p->set_storage_class(t); }
	bool is_ptr() const		{ return p->is_ptr(); }
	bool is_su() const		{ return p->is_su(); }
	bool is_function() const	{ return p->is_function(); }
	bool is_typedef() const		{ return p->is_typedef(); }
	bool is_valid() const		{ return p->is_valid(); }
	bool is_basic() const		{ return p->is_basic(); }
	bool is_padbit() const		{ return p->is_padbit(); }
	bool is_unused_attr() const	{ return p->is_unused_attr(); }
	bool is_abstract() const	{ return p->is_abstract(); }
	bool is_incomplete() const	{ return p->is_incomplete(); }
	bool is_array() const		{ return p->is_array(); }
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
	Id const* member(unsigned n) const	// Structure and union
					{ return p->member(n); }
	friend Type merge(Type a, Type b) { return a.p->merge(b.p->tobasic()); }
};


inline void Tbasic::set_storage_class(Type t) { sclass.set_storage_class(t); }

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
