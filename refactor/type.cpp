/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: type.cpp,v 1.36 2003/09/29 18:10:08 dds Exp $
 */

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <list>

#include "ytab.h"

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "error.h"
#include "ptoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "type2.h"
#include "debug.h"



Type&
Type::operator=(const Type& rhs)
{
	// The operation sequence handles self-assignment
	rhs.p->use++;
	if (--p->use == 0)
		delete p;
	p = rhs.p;
	return (*this);
}

Type
Type_node::subscript() const
{
	/*
	 * @error
	 * The object being subscripted using the <code>[]</code>
	 * operator is not an array or a pointer
	 */
	Error::error(E_ERR, "subscript not on array or pointer");
	if (DP())
		this->print(cerr);
	return basic(b_undeclared);
}

const Stab& 
Type_node::get_members_by_name() const
{
	static Stab dummy;

	Error::error(E_INTERNAL, "get_members_by_name: not structure or union");
	this->print(cout);
	return dummy;
}

const vector <Id>&
Type_node::get_members_by_ordinal() const
{
	static vector<Id> dummy;

	Error::error(E_INTERNAL, "get_members_by_ordinal: not structure or union");
	this->print(cout);
	return dummy;
}

Type
Type_node::call() const
{
	/*
	 * @error
	 * The object used as a function to be called is not a function
	 * or a pointer to a function
	 */
	Error::error(E_ERR, "object is not a function");
	return basic(b_undeclared);
}

Type
Tidentifier::call() const
{
	// Undeclared identifiers f when called are declared as int f(...)
	obj_define(this->get_token(), function_returning(basic(b_int)));
	/*
	 * @error
	 * An undeclared identifier is used as a function
	 */
	Error::error(E_WARN, "assuming declaration int " + this->get_name() + "(...)");
	return basic(b_int);
}


const Ctoken& 
Type_node::get_token() const
{
	static Ctoken c;
	Error::error(E_INTERNAL, "attempt to get token value of a non-identifier");
	this->print(cerr);
	return c;
}

Type
Type_node::clone() const
{
	Error::error(E_INTERNAL, "unable to clone type");
	this->print(cerr);
	return Type();
}

Tbasic *
Type_node::tobasic()
{
	Error::error(E_INTERNAL, "unknown tobasic() conversion");
	this->print(cerr);
	return NULL;
}

const string&
Type_node::get_name() const
{
	static string s("unknown");
	Error::error(E_INTERNAL, "attempt to get the name of a non-identifier");
	this->print(cerr);
	return s;
}

Type
Type_node::deref() const
{
	/*
	 * @error
	 * An attempt was made to dereference an element that is not a pointer
	 */
	Error::error(E_ERR, "illegal pointer dereference");
	if (DP())
		this->print(cerr);
	return basic(b_undeclared);
}


Type
Type_node::type() const
{
	Error::error(E_INTERNAL, "object is not a typedef or identifier");
	this->print(cerr);
	return basic(b_undeclared);
}


enum e_storage_class 
Type_node::get_storage_class() const
{
	Error::error(E_INTERNAL, "object has no storage class");
	this->print(cerr);
	return c_unspecified;
}

qualifiers_t
Type_node::get_qualifiers() const
{
	Error::error(E_INTERNAL, "object has no associated qualifiers");
	this->print(cerr);
	return q_none;
}

const Id *
Type_node::member(const string& s) const
{
	Error::error(E_ERR, "invalid member access: not a structure or union");
	if (DP())
		this->print(cerr);
	return NULL;
}

const Id *
Type_node::member(unsigned n) const
{
	Error::error(E_ERR, "invalid member access: not a structure or union");
	if (DP())
		this->print(cerr);
	return NULL;
}

const Id *
Tincomplete::member(const string& s) const
{
	const Id *id = tag_lookup(scope_level, t.get_name());
	if (DP() && id) {
		cout << "Incomplete member access of " << t.get_name() << "." << s;
		cout << " Type: " << id->get_type() << "\n";
	}
	if (!id || id->get_type().is_incomplete()) {
		/*
		 * @error
		 * The member access for a structure or union is applied
		 * on an object with an incomplete definition
		 */
		Error::error(E_ERR, string("member access in incomplete struct/union: ") + t.get_name());
		if (DP())
			this->print(cerr);
		return NULL;
	} else
		return id->get_type().member(s);
}

Type
basic(enum e_btype t, enum e_sign s, enum e_storage_class sc, qualifiers_t q)
{
	return Type(new Tbasic(t, s, sc, q));
}

Type
array_of(Type t)
{
	return Type(new Tarray(t));
}

Type
pointer_to(Type t)
{
	return Type(new Tpointer(t));
}

Type
function_returning(Type t)
{
	return Type(new Tfunction(t));
}

Type
enum_tag()
{
	return Type(new Tenum());
}

Type
struct_union(const Token &tok, const Type &typ, const Type &spec)
{
	return Type(new Tsu(tok, typ, spec));
}

Type
struct_union(const Type &spec)
{
	return Type(new Tsu(spec));
}

Type
struct_union()
{
	return Type(new Tsu());
}

Type
identifier(const Ctoken& t)
{
	return Type(new Tidentifier(t));
}

Type
incomplete(const Ctoken& t, int l)
{
	return Type(new Tincomplete(t, l));
}

Type
label()
{
	return Type(new Tlabel());
}

void
Type_node::print(ostream &o) const
{
	o << "Unknown type node";
}

void
Tstorage::print(ostream &o) const
{
	switch (sclass) {
	case c_unspecified: break;
	case c_typedef: o << "typedef "; break;
	case c_extern: o << "extern "; break;
	case c_static: o << "static "; break;
	case c_auto: o << "auto "; break;
	case c_register: o << "register "; break;
	case c_enum: o << "enum_const "; break;
	}
}

void
QType_node::print(ostream &o) const
{
	if (qualifiers & q_const) o << "const ";
	if (qualifiers & q_volatile) o << "volatile ";
	if (qualifiers & q_unused) o << "__attribute__(unused) ";
}

void
Tbasic::print(ostream &o) const
{
	sclass.print(o);
	QType_node::print(o);

	switch (sign) {
	case s_none: break;
	case s_signed: o << "signed "; break;
	case s_unsigned: o << "unsigned "; break;
	}

	switch (type) {
	case b_abstract: o << "ABSTRACT "; break;
	case b_void: o << "void "; break;
	case b_char: o << "char "; break;
	case b_short: o << "short "; break;
	case b_int: o << "int "; break;
	case b_long: o << "long "; break;
	case b_llong: o << "long long "; break;
	case b_float: o << "float "; break;
	case b_double: o << "double "; break;
	case b_ldouble: o << "long double "; break;
	case b_padbit: o << "padbit "; break;
	case b_undeclared: o << "UNDECLARED "; break;
	}
}

void
Tarray::print(ostream &o) const
{
	o << "array of " << of;
}

void
Tpointer::print(ostream &o) const
{
	o << "pointer to " << to;
}

void
Tfunction::print(ostream &o) const
{
	o << "function returning " << returning;
}

void
Tlabel::print(ostream &o) const
{
	o << "label ";
}

void
Tsu::print(ostream &o) const
{
	sclass.print(o);
	QType_node::print(o);

	o << "struct/union " << members_by_name;
}

void
Tincomplete::print(ostream &o) const
{
	sclass.print(o);
	QType_node::print(o);

	o << "struct/union " << t.get_name() << "(incomplete) ";
}

void
Tenum::print(ostream &o) const
{
	sclass.print(o);
	QType_node::print(o);

	o << "enum ";
}

void
Tidentifier::print(ostream &o) const
{
	o << t.get_name() << ":" << of;
}

/*
 * Merge two basic types constisting of sign, type specifiers, and
 * storage class, returning the composite type
 * See ANSI 3.5.2
 */
Type
Tbasic::merge(Tbasic *b)
{
	enum e_btype t;
	enum e_sign s;
	enum e_storage_class c;
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (this->type == b_abstract || this->type == b_undeclared)
		t = b->type;
	else if (b->type == b_abstract || b->type == b_undeclared)
		t = this->type;
	else if ((this->type == b_long && b->type == b_int) ||
	         (this->type == b_int && b->type == b_long))
		t = b_long;
	else if ((this->type == b_short && b->type == b_int) ||
	         (this->type == b_int && b->type == b_short))
		t = b_short;
	else if ((this->type == b_long && b->type == b_double) ||
	         (this->type == b_double && b->type == b_long))
		t = b_ldouble;
	else if ((this->type == b_long && b->type == b_long) ||
	         (this->type == b_long && b->type == b_long) ||
	         (this->type == b_llong && b->type == b_int) ||
	         (this->type == b_int && b->type == b_llong))
		t = b_llong;		// Extension to ANSI
	else {
		/*
		 * @error
		 * The type specifiers used can not be combined
		 * (e.g. <code>double char</code>)
		 */
		Error::error(E_ERR, "illegal combination of type specifiers");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
		t = b_undeclared;
	}

	if (this->sign == s_none)
		s = b->sign;
	else if (b->sign == s_none)
		s = this->sign;
	else {
		/*
		 * @error
		 * The signedness specifiers used can not be combined
		 * (e.g. <code>unsigned signed</code>)
		 */
		Error::error(E_WARN, "illegal combination of sign specifiers");
		s = s_none;
	}

	if (s != s_none && (t == b_float || t == b_double || t == b_ldouble)) {
		/*
		 * @error
		 * A signedness specification was given on a non-integral type
		 */
		Error::error(E_WARN, "sign specification on non-integral type - ignored");
		s = s_none;
	}

	if (this->sclass.get_storage_class() == c_unspecified)
		c = b->sclass.get_storage_class();
	else if (b->sclass.get_storage_class() == c_unspecified)
		c = this->sclass.get_storage_class();
	else {
		/*
		 * @error
		 * More than one storage class was given for the same
		 * object
		 */
		Error::error(E_ERR, "at most one storage class can be specified");
		c = this->sclass.get_storage_class();
	}

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	if (DP()) {
		cout << "merge a=" << Type(this->clone()) << "\nmerge b=" << 
			Type(b) << "\nmerge r=" <<  Type(basic(t, s, c, q)) << "\n";
	}
	return basic(t, s, c, q);
}

Type
Tsu::merge(Tbasic *b)
{
	enum e_storage_class c;
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (!b->is_abstract()) {
		/*
		 * @error
		 * The type specifiers used can not be applied on an aggregate
		 * (e.g. <code>int struct</code>)
		 */
		Error::error(E_ERR, "illegal application of type specifiers on an aggregate");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
	}

	if (this->get_storage_class() == c_unspecified)
		c = b->get_storage_class();
	else if (b->get_storage_class() == c_unspecified)
		c = this->get_storage_class();
	else
		c = this->get_storage_class();

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	return Type(new Tsu(members_by_name, members_by_ordinal, default_specifier.clone(), c, q));
}

Type
Tpointer::merge(Tbasic *b)
{
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (!b->is_abstract() ||
	    b->get_storage_class() != c_unspecified) {
		/*
		 * @error
		 * The type specifier or storage class can not be applied on 
		 * a pointer
		 */
		Error::error(E_ERR, "illegal application of type attributes on a pointer");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
	}

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	return Type(new Tpointer(to, q));
}

Type
Tarray::merge(Tbasic *b)
{
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (!b->is_abstract() ||
	    b->get_storage_class() != c_unspecified) {
		/*
		 * @error
		 * The type specifier or storage class can not be applied on 
		 * an array
		 */
		Error::error(E_ERR, "illegal application of type attributes on an array");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
	}

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	return Type(new Tarray(of, q));
}

Type
Tincomplete::merge(Tbasic *b)
{
	enum e_storage_class c;
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (!b->is_abstract()) {
		Error::error(E_ERR, "illegal application of type specifiers on an aggregate");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
	}

	if (this->get_storage_class() == c_unspecified)
		c = b->get_storage_class();
	else if (b->get_storage_class() == c_unspecified)
		c = this->get_storage_class();
	else
		c = this->get_storage_class();

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	return Type(new Tincomplete(t, c, scope_level, q));
}

Type
Tenum::merge(Tbasic *b)
{
	enum e_storage_class c;
	enum e_qualifier q;

	if (b == NULL)
		return Type_node::merge(b);
	if (!b->is_abstract()) {
		/*
		 * @error
		 * The type specifiers used can not be applied on an enumaration
		 * (e.g. <code>int enum</code>)
		 */
		Error::error(E_ERR, "illegal application of type specifiers on an enumeration");
		if (DP())
			cout << "merge a=" << Type(this) << "\nmerge b=" << Type(b) << "\n";
	}

	if (this->get_storage_class() == c_unspecified)
		c = b->get_storage_class();
	else if (b->get_storage_class() == c_unspecified)
		c = this->get_storage_class();
	else
		c = this->get_storage_class();

	q = (enum e_qualifier)(this->get_qualifiers() | b->get_qualifiers());
	return Type(new Tenum(c, q));
}


Type
Type_node::merge(Tbasic *b)
{
	/*
	 * @error
	 * A basic type, storage class or type specified was specified in
	 * an invalid underlying object
	 */
	Error::error(E_ERR, "invalid application of basic type, storage class, or type specifier");
	if (DP()) {
		cout << "a=";
		this->print(cout);
		cout << "\nb=";
		b->print(cout);
		cout << "\n";
	}
	return basic();
}

void
Type_node::set_abstract(Type t)
{
	/*
	 * @error
	 * An attempt was made to specify a type on an object that
	 * did not allow this specification
	 */
	Error::error(E_ERR, "invalid type specification");
	if (DP())
		this->print(cerr);
}

void
Tbasic::set_abstract(Type t)
{
	if (! (type == b_padbit ||
	       (type == b_abstract &&
	        sign ==  s_none &&
		sclass.get_storage_class() == c_unspecified)))
		/*
		 * @error
		 * An attempt was made to specify a type on a basic object that
		 * did not allow this specification, because that specification
		 * was already given
		 */
		Error::error(E_ERR, "invalid type specification for basic type");
	set_qualifiers(t.get_qualifiers());
	if (DP())
		this->print(cerr);
}

void
Tarray::set_abstract(Type t)
{
	if (of.is_basic()) {
		if (of.is_abstract())
			of = t;
		else {
			/*
			 * @error
			 * The underlying array object for which a type is
			 * specified is not an abstract type
			 */
			Error::error(E_ERR, "array not an abstract type");
			cerr << "[" << of << "]\n";
		}
	} else
		of.set_abstract(t);
}

void
Tpointer::set_abstract(Type t)
{
	if (to.is_basic()) {
		if (to.is_abstract())
			to = t;
		else {
			/*
			 * @error
			 * The underlying pointer object for which a type is
			 * specified is not an abstract type
			 */
			Error::error(E_ERR, "pointer not an abstract type");
			cerr << "[" << to << "]\n";
		}
	} else
		to.set_abstract(t);
}

void
Tfunction::set_abstract(Type t)
{
	if (returning.is_basic()) {
		if (returning.is_abstract())
			returning = t;
		else
			Error::error(E_ERR, "pointer not an abstract type");
	} else
		returning.set_abstract(t);
}

void
Tidentifier::set_abstract(Type t)
{
	if (of.is_basic()) {
		if (of.is_abstract())
			of = t;
		else
			Error::error(E_ERR, "pointer not an abstract type");
	} else
		of.set_abstract(t);
	if (DP())
		this->print(cerr);
}

void
Type::declare()
{
	obj_define(this->get_token(), this->type());
}

Type 
Tbasic::clone() const
{
	return Type(new Tbasic(type, sign, sclass.get_storage_class(), get_qualifiers()));
}

void
Tstorage::set_storage_class(Type t)
{
	enum e_storage_class newclass = t.get_storage_class();

	if (sclass != c_unspecified &&
	    sclass != c_typedef &&
	    newclass != c_unspecified)
		/*
		 * @error
		 * Incompatible storage classes were specified in a single
		 * type declaration
		 */
		Error::error(E_ERR, "multiple storage classes in type declaration");
	// if sclass is already e.g. extern and t is just volatile don't destry sclass
	if (sclass == c_unspecified || sclass == c_typedef)
		sclass = newclass;
}

void
Type_node::set_storage_class(Type t)
{
	Error::error(E_INTERNAL, "object can not set storage class");
	this->print(cerr);
}

void
Type_node::add_qualifiers(Type t)
{
	Error::error(E_INTERNAL, "object can not add qualifiers");
	this->print(cerr);
}

void
Type_node::add_member(const Token &tok, const Type &typ)
{
	Error::error(E_INTERNAL, "add_member: object is not a structure/union");
	this->print(cerr);
}

Type
Type_node::get_default_specifier() const
{
	Error::error(E_INTERNAL, "get_default_specifier: object is not a structure/union");
	this->print(cerr);
	return basic();
}

void
Type_node::merge_with(Type t)
{
	Error::error(E_INTERNAL, "merge_with: object is not a structure/union");
	cerr << "A: " << Type(this) << "\n";
	cerr << "B: " << t << "\n";
}

#ifdef NODE_USE_PROFILE
int Type_node::count;

int Type_node::get_count()
{
	return count;
}
#endif
