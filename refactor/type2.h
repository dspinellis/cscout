/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The derived types of the type-system structure
 * Tsu (struct/union) depends on Stab which depends on Type, so we
 * split the type file into two.
 *
 * $Id: type2.h,v 1.2 2001/09/22 07:38:52 dds Exp $
 */

#ifndef TYPE2_
#define TYPE2_

// Array of ...
class Tarray: public Type_node {
private:
	Type of;
public:
	Tarray(Type t) : of(t) {}
	Type clone() const { return Type(new Tarray(of.clone())); }
	Type subscript() const { return of; }
	bool is_ptr() { return true; }
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { of.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return of.get_storage_class(); }
};

// Pointer to ...
class Tpointer: public Type_node {
private:
	Type to;
public:
	Tpointer(Type t) : to(t) {}
	Type clone() const { return Type(new Tpointer(to.clone())); }
	Type deref() const { return to; }
	bool is_ptr() { return true; }
	void set_storage_class(Type t) { to.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return to.get_storage_class(); }
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Function returning ...
class Tfunction: public Type_node {
private:
	Type returning;
public:
	Tfunction(Type t) : returning(t) {}
	Type clone() const { return Type(new Tfunction(returning.clone())); }
	Type call() const { return returning; }
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { returning.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return returning.get_storage_class(); }
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

class Stab;

// Structure or Union
class Tsu: public Type_node {
private:
	Stab members;
	Type default_specifier;	// Used while declaring a series of members
	Tstorage sclass;
public:
	Tsu(const Token &tok, const Type &typ, const Type &spec) 
		{ members.define(tok, typ); default_specifier = spec; }
	Tsu(const Type &spec) { default_specifier = spec; }
	Tsu(const Stab& m, const Type& ds, const Tstorage& sc) :
		members(m), default_specifier(ds), sclass(sc) {}
	Type clone() const { return Type(new Tsu(members, default_specifier, sclass)); }
	void add_member(const Token &tok, const Type &typ)
		{ members.define(tok, typ); }
	Type get_default_specifier() const { return default_specifier; }
	void merge_with(Type t) { members.merge_with(t.get_members()) ; }
	Id const* member(const string& s) const 
		{ return members.lookup(s); }
	const Stab& get_members() const { return members; }
	void print(ostream &o) const;
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	void set_storage_class(Type t) { sclass.set_storage_class(t); };
};

// Identifier; not really a type, it is returned by the lexical analyser
// It is also the type used to represent undeclared identifiers
class Tidentifier: public Type_node {
private:
	Ctoken t;
	Type of;			// Identifying a given type
public:
	Tidentifier(const Ctoken& tok) : t(tok), of(basic()) {}
	Tidentifier(const Ctoken& tok, Type typ) : t(tok), of(typ) {}
	Type clone() const { return Type(new Tidentifier(t, of.clone())); }
	const Ctoken& get_token() const { return t; }
	const string& get_name() const { return t.get_name(); }
	Type type() const { return of; }
	Type call() const;			// Function (undeclared)
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { of.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return of.get_storage_class(); }
};

// Goto label
class Tlabel: public Type_node {
public:
	Tlabel() {}
	void print(ostream &o) const;
};


#endif // TYPE2_
