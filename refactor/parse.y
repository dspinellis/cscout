/*
 * (C) Copyright 2001 Diomidis Spinellis.
 * Portions Copyright (c)  1989,  1990  James  A.  Roskind
 * Based on work by James A. Roskind; see comments at the end of this file.
 * Grammar obtained from http://www.empathy.com/pccts/roskind.html 
 *
 * Declarations and type checking engine.  
 * Note that for the purposes of this work we do not
 * need to keep precise track of types, esp. implicit arithmetic conversions.
 * Type checking is used:
 * 1) To identify the structure or union to use for member access
 * 2) As a sanity check for (1)
 * 3) To avoid mistages caused by ommitting arbitrary part of the type checking 
 *    mechanism
 * 4) To handle typedefs
 *
 * $Id: parse.y,v 1.43 2002/09/17 16:20:37 dds Exp $
 *
 */

%include ytoken.h

%start translation_unit

%{
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
#include "metrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"		// get_fileid()
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

void parse_error(char *s)
{
	Error::error(E_ERR, "syntax error");
}


#define YYSTYPE_CONSTRUCTOR

%}


%type <t> IDENTIFIER
%type <t> TYPEDEF_NAME
%type <t> identifier_or_typedef_name
%type <t> member_name
%type <t> constant
%type <t> primary_expression
%type <t> postfix_expression
%type <t> unary_expression
%type <t> cast_expression
%type <t> multiplicative_expression
%type <t> additive_expression
%type <t> shift_expression
%type <t> relational_expression
%type <t> equality_expression
%type <t> and_expression
%type <t> exclusive_or_expression
%type <t> inclusive_or_expression
%type <t> logical_and_expression
%type <t> logical_or_expression
%type <t> conditional_expression
%type <t> constant_expression
%type <t> assignment_expression
%type <t> string_literal_list
%type <t> comma_expression

%type <t> basic_type_name
%type <t> storage_class
%type <t> declaration_qualifier_list
%type <t> type_qualifier_list
%type <t> declaration_qualifier
%type <t> type_qualifier
%type <t> basic_declaration_specifier
%type <t> basic_type_specifier
%type <t> type_name
%type <t> type_specifier
%type <t> default_declaring_list
%type <t> declaring_list
%type <t> typedef_declaration_specifier
%type <t> typedef_type_specifier
%type <t> declaration_specifier
%type <t> sue_type_specifier
%type <t> sue_declaration_specifier
%type <t> member_declaration_list
%type <t> member_declaration
%type <t> member_default_declaring_list
%type <t> member_declaring_list
%type <t> member_declarator
%type <t> member_identifier_declarator
%type <t> elaborated_type_name
%type <t> aggregate_name
%type <t> enum_name
%type <t> old_function_declarator
%type <t> postfix_old_function_declarator

%type <t> declarator
%type <t> typedef_declarator
%type <t> parameter_typedef_declarator
%type <t> clean_typedef_declarator
%type <t> clean_postfix_typedef_declarator
%type <t> paren_typedef_declarator
%type <t> paren_postfix_typedef_declarator
%type <t> simple_paren_typedef_declarator
%type <t> paren_identifier_declarator
%type <t> array_abstract_declarator
%type <t> postfixing_abstract_declarator
%type <t> unary_abstract_declarator
%type <t> postfix_abstract_declarator
%type <t> abstract_declarator
%type <t> postfix_identifier_declarator
%type <t> unary_identifier_declarator
%type <t> identifier_declarator

%%

/* This refined grammar resolves several typedef ambiguities  in  the
draft  proposed  ANSI  C  standard  syntax  down  to  1  shift/reduce
conflict, as reported by a YACC process.  Note  that  the  one  shift
reduce  conflicts  is the traditional if-if-else conflict that is not
resolved by the grammar.  This ambiguity can  be  removed  using  the
method  described in the Dragon Book (2nd edition), but this does not
appear worth the effort.

There was quite a bit of effort made to reduce the conflicts to  this
level,  and  an  additional effort was made to make the grammar quite
similar to the C++ grammar being developed in  parallel.   Note  that
this grammar resolves the following ANSI C ambiguity as follows:

ANSI  C  section  3.5.6,  "If  the [typedef name] is redeclared at an
inner scope, the type specifiers shall not be omitted  in  the  inner
declaration".   Supplying type specifiers prevents consideration of T
as a typedef name in this grammar.  Failure to supply type specifiers
forced the use of the TYPEDEF_NAME as a type specifier.

ANSI C section 3.5.4.3, "In a parameter declaration, a single typedef
name in parentheses is  taken  to  be  an  abstract  declarator  that
specifies  a  function  with  a  single  parameter,  not as redundant
parentheses around the identifier".  This is extended  to  cover  the
following cases:

typedef float T;
int noo(const (T[5]));
int moo(const (T(int)));
...

Where  again the '(' immediately to the left of 'T' is interpreted as
being the start of a parameter type list,  and  not  as  a  redundant
paren around a redeclaration of T.  Hence an equivalent code fragment
is:

typedef float T;
int noo(const int identifier1 (T identifier2 [5]));
int moo(const int identifier1 (T identifier2 (int identifier3)));
...

*/


/* CONSTANTS */
constant:
        INT_CONST
			{ $$ = basic(b_int); }
        | FLOAT_CONST
			{ $$ = basic(b_float); }
        /* We are not including ENUMERATIONconstant here  because  we
        are  treating  it like a variable with a type of "enumeration
        constant".  */
        ;

string_literal_list:
                STRING_LITERAL
			{ $$ = array_of(basic(b_char)); }
                | string_literal_list STRING_LITERAL
			{ $$ = $1; }
                ;


/************************* EXPRESSIONS ********************************/
primary_expression:
        IDENTIFIER  /* We cannot use a typedef name as a variable */
			{
				Id const *id = obj_lookup($1.get_name());
				if (id) {
					unify(id->get_token(), $1.get_token());
					$$ = id->get_type();
				} else {
					Error::error(E_WARN, "undeclared identifier: " + $1.get_name());
					$$ = $1;
				}
			}
        | constant
        | string_literal_list
        | '(' comma_expression ')'
			{ $$ = $2; }
        ;

postfix_expression:
        primary_expression
        | postfix_expression '[' comma_expression ']'
			{ $$ = $1.subscript(); }
        | postfix_expression '(' ')'
			{ $$ = $1.call(); }
        | postfix_expression '(' argument_expression_list ')'
			{ $$ = $1.call(); }
        | postfix_expression '.'   member_name
			{
				Id const *id = $1.member($3.get_name());
				if (id) {
					$$ = id->get_type();
					if (DP())
						cout << ". returns " << $$ << "\n";
					assert(id->get_name() == $3.get_name());
					unify($3.get_token(), id->get_token());
				} else {
					Error::error(E_ERR, "structure or union does not have a member " + $3.get_name());
					$$ = basic(b_undeclared);
				}
			}
        | postfix_expression PTR_OP member_name
			{
				Id const *id = ($1.deref()).member($3.get_name());
				if (id) {
					$$ = id->get_type();
					assert(id->get_name() == $3.get_name());
					unify($3.get_token(), id->get_token());
				} else {
					Error::error(E_ERR, "structure or union does not have a member " + $3.get_name());
					$$ = basic(b_undeclared);
				}
			}
        | postfix_expression INC_OP
			{ $$ = $1; }
        | postfix_expression DEC_OP
			{ $$ = $1; }
        ;

member_name:
        IDENTIFIER
        | TYPEDEF_NAME
        ; /* Default rules */

argument_expression_list:
        assignment_expression
        | argument_expression_list ',' assignment_expression
        ;

unary_expression:
        postfix_expression
        | INC_OP unary_expression
			{ $$ = $2; }
        | DEC_OP unary_expression
			{ $$ = $2; }
        | arith_unary_operator cast_expression
			{ $$ = $2; }
        | '&' cast_expression
			{ $$ = pointer_to($2); }
        | '*' cast_expression
			{ $$ = $2.deref(); }
        | SIZEOF unary_expression
			{ $$ = basic(b_int); }
        | SIZEOF '(' type_name ')'
			{ $$ = basic(b_int); }
        ;

arith_unary_operator:
        '+'
        | '-'
        | '~'
        | '!'
        ;

cast_expression:
        unary_expression
        | '(' type_name ')' cast_expression
		{ 
			$$ = $2;  
			if (DP())
				cout << "cast to " << $2 << "\n"; 
		}
        ;

multiplicative_expression:
        cast_expression
        | multiplicative_expression '*' cast_expression
		{ $$ = $1; }
        | multiplicative_expression '/' cast_expression
		{ $$ = $1; }
        | multiplicative_expression '%' cast_expression
		{ $$ = $1; }
        ;

additive_expression:
        multiplicative_expression
        | additive_expression '+' multiplicative_expression
			{ $$ = $1; }
        | additive_expression '-' multiplicative_expression
			{
				if ($1.is_ptr() && $3.is_ptr())
					$$ = basic(b_int);
				else 
					$$ = $1;
			}
        ;

shift_expression:
        additive_expression
        | shift_expression LEFT_OP additive_expression
		{ $$ = $1; }
        | shift_expression RIGHT_OP additive_expression
		{ $$ = $1; }
        ;

relational_expression:
        shift_expression
        | relational_expression '<' shift_expression
			{ $$ = basic(b_int); }
        | relational_expression '>' shift_expression
			{ $$ = basic(b_int); }
        | relational_expression LE_OP shift_expression
			{ $$ = basic(b_int); }
        | relational_expression GE_OP shift_expression
			{ $$ = basic(b_int); }
        ;

equality_expression:
        relational_expression
        | equality_expression EQ_OP relational_expression
			{ $$ = basic(b_int); }
        | equality_expression NE_OP relational_expression
			{ $$ = basic(b_int); }
        ;

and_expression:
        equality_expression
        | and_expression '&' equality_expression
		{ $$ = $1; }
        ;

exclusive_or_expression:
        and_expression
        | exclusive_or_expression '^' and_expression
		{ $$ = $1; }
        ;

inclusive_or_expression:
        exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression
		{ $$ = $1; }
        ;

logical_and_expression:
        inclusive_or_expression
        | logical_and_expression AND_OP inclusive_or_expression
			{ $$ = basic(b_int); }
        ;

logical_or_expression:
        logical_and_expression
        | logical_or_expression OR_OP logical_and_expression
			{ $$ = basic(b_int); }
        ;

conditional_expression:
        logical_or_expression
        | logical_or_expression '?' comma_expression ':' conditional_expression
			{ $$ = $3; }
        ;

assignment_expression:
        conditional_expression
        | unary_expression assignment_operator assignment_expression
		{ $$ = $1; }
        ;

assignment_operator:
        '='
        | MUL_ASSIGN
        | DIV_ASSIGN
        | MOD_ASSIGN
        | ADD_ASSIGN
        | SUB_ASSIGN
        | LEFT_ASSIGN
        | RIGHT_ASSIGN
        | AND_ASSIGN
        | XOR_ASSIGN
        | OR_ASSIGN
        ;

comma_expression:
        assignment_expression
	/* GCC extension (this introduces a reduce / reduce conflict) */
	| compound_statement
			{ $$ = basic(); }
        | comma_expression ',' assignment_expression
			{ $$ = $3; }
        ;

constant_expression:
        conditional_expression
        ; /* Default rules */

    /* The following was used for clarity */
comma_expression_opt:
        /* Nothing */
        | comma_expression
        ;


/******************************* DECLARATIONS *********************************/

    /* The following is different from the ANSI C specified  grammar.
    The  changes  were  made  to  disambiguate  typedef's presence in
    declaration_specifiers (vs.  in the declarator for redefinition);
    to allow struct/union/enum tag declarations without  declarators,
    and  to  better  reflect the parsing of declarations (declarators
    must be combined with declaration_specifiers ASAP  so  that  they
    are visible in scope).

    Example  of  typedef  use  as either a declaration_specifier or a
    declarator:

      typedef int T;
      struct S { T T;}; / * redefinition of T as member name * /

    Example of legal and illegal statements detected by this grammar:

      int; / * syntax error: vacuous declaration * /
      struct S;  / * no error: tag is defined or elaborated * /

    Example of result of proper declaration binding:

        int a=sizeof(a); / * note that "a" is declared with a type  in
            the name space BEFORE parsing the initializer * /

        int b, c[sizeof(b)]; / * Note that the first declarator "b" is
             declared  with  a  type  BEFORE the second declarator is
             parsed * /

    */

declaration:
        sue_declaration_specifier ';'
        | sue_type_specifier ';'
        | declaring_list ';'
        | default_declaring_list ';'
        ;

    /* Note that if a typedef were  redeclared,  then  a  declaration
    specifier must be supplied */

default_declaring_list:  /* Can't  redeclare typedef names */
	/* static volatile @ a[3] @ = { 1, 2, 3} */
        declaration_qualifier_list identifier_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
	/* volatile @ a[3] @ = { 1, 2, 3} */
        | type_qualifier_list identifier_declarator
		{
			$2.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
        | default_declaring_list ',' identifier_declarator
		{
			$3.set_abstract($1);
			$3.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
        ;

declaring_list:
	/* static int @ FILE @ = 42 (note reuse of typedef name) */
        declaration_specifier declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
	/* int @ FILE @ = 42 */
        | type_specifier declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
        | declaring_list ',' declarator
		{
			$3.set_abstract($1);
			$3.declare();
		}
						 initializer_opt
		{ $$ = $1; /* Pass-on qualifier */ }
        ;

/* Includes storage class */
declaration_specifier:
        basic_declaration_specifier          /* Arithmetic or void */
        | sue_declaration_specifier          /* struct/union/enum */
        | typedef_declaration_specifier      /* typedef */
        ; /* Default rules */

type_specifier:
        basic_type_specifier                 /* Arithmetic or void */
        | sue_type_specifier                 /* Struct/Union/Enum */
        | typedef_type_specifier             /* Typedef */
        ; /* Default rules */


/* e.g. typedef static volatile const */
declaration_qualifier_list:  /* const/volatile, AND storage class */
        storage_class
        | type_qualifier_list storage_class
			{ $$ = $2; }
        | declaration_qualifier_list declaration_qualifier
			{ $$ = merge($1, $2); }
        ;

/* e.g. const volatile */
type_qualifier_list:
        type_qualifier
        | type_qualifier_list type_qualifier
        ; /* default rules */

/* One of: static extern typedef register auto const volatile */
declaration_qualifier:
        storage_class
        | type_qualifier                  /* const or volatile */
        ; /* default rules */

type_qualifier:
        TCONST		{ $$ = basic(); }
        | VOLATILE	{ $$ = basic(); }
        ;

basic_declaration_specifier:      /* Storage Class+Arithmetic or void */
        declaration_qualifier_list basic_type_name	/* static, int */
			{ $$ = merge($1, $2); }
        | basic_type_specifier  storage_class		/* int, static */
			{ $$ = merge($1, $2); }
	/* static int, volatile */
        | basic_declaration_specifier declaration_qualifier
			{ $$ = merge($1, $2); }
	/* static long, int */
        | basic_declaration_specifier basic_type_name
			{ $$ = merge($1, $2); }
        ;

basic_type_specifier:
        basic_type_name            /* Arithmetic or void */
        | type_qualifier_list  basic_type_name		/* const, int */
			{ $$ = $2; }
        | basic_type_specifier type_qualifier		/* int, volatile */
			{ $$ = $1; }
        | basic_type_specifier basic_type_name		/* long, int */
			{ $$ = merge($1, $2); }
        ;

sue_declaration_specifier:          /* Storage Class + struct/union/enum */
	/* static const @ struct foo {int a;} */
        declaration_qualifier_list  elaborated_type_name
		{ $$ = $2.clone(); $$.set_storage_class($1);}
        | sue_type_specifier        storage_class
		{ $$ = $1.clone(); $$.set_storage_class($2); }
        | sue_declaration_specifier declaration_qualifier
		{ $$ = $1; }
        ;

sue_type_specifier:
        elaborated_type_name              /* struct/union/enum */
        | type_qualifier_list elaborated_type_name
		{ $$ = $2; }
        | sue_type_specifier  type_qualifier
        ;


typedef_declaration_specifier:       /* Storage Class + typedef types */
        typedef_type_specifier          storage_class
		{
			$1.set_storage_class($2);
			$$ = $1;
		}
        | declaration_qualifier_list    TYPEDEF_NAME
		{ 
			Id const *id = obj_lookup($2.get_name());
			assert(id);	// If it's a typedef it can be found
			unify(id->get_token(), $2.get_token());
			$$ = id->get_type().clone();
			$$.set_storage_class($1);
		}
        | typedef_declaration_specifier declaration_qualifier
		{
			$$ = $1;
		}
        ;

typedef_type_specifier:              /* typedef types */
        TYPEDEF_NAME
		{ 
			Id const *id = obj_lookup($1.get_name());
			assert(id);	// If it's a typedef it can be found
			unify(id->get_token(), $1.get_token());
			$$ = id->get_type().clone();
			$$.set_storage_class(basic(b_abstract, s_none, c_unspecified));
		}
        | type_qualifier_list    TYPEDEF_NAME
		{ 
			Id const *id = obj_lookup($2.get_name());
			assert(id);	// If it's a typedef it can be found
			unify(id->get_token(), $2.get_token());
			$$ = id->get_type().clone();
			$$.set_storage_class(basic(b_abstract, s_none, c_unspecified));
		}
        | typedef_type_specifier type_qualifier
		{ $$ = $1; }
        ;

storage_class:
        TYPEDEF		{ $$ = basic(b_abstract, s_none, c_typedef); }
        | EXTERN	{ $$ = basic(b_abstract, s_none, c_extern); }
        | STATIC	{ $$ = basic(b_abstract, s_none, c_static); }
        | AUTO		{ $$ = basic(b_abstract, s_none, c_auto); }
        | REGISTER	{ $$ = basic(b_abstract, s_none, c_register); }
        ;

basic_type_name:
        INT		{ $$ = basic(b_int); }
        | CHAR		{ $$ = basic(b_char); }
        | SHORT		{ $$ = basic(b_short); }
        | LONG		{ $$ = basic(b_long); }
        | FLOAT		{ $$ = basic(b_float); }
        | DOUBLE	{ $$ = basic(b_double); }
        | SIGNED	{ $$ = basic(b_abstract, s_signed); }
        | UNSIGNED	{ $$ = basic(b_abstract, s_unsigned); }
        | TVOID		{ $$ = basic(b_void); }
        | TYPEOF '(' IDENTIFIER ')'
			{
				Id const *id = obj_lookup($3.get_name());
				if (id) {
					unify(id->get_token(), $3.get_token());
					$$ = id->get_type();
				} else {
					Error::error(E_WARN, "undeclared identifier: " + $3.get_name());
					$$ = $3;
				}
			}
        ;

elaborated_type_name:
        aggregate_name
        | enum_name
        ; /* Default rules */

aggregate_name:
        aggregate_key '{'  member_declaration_list '}'
		{ $$ = $3; }
        | aggregate_key identifier_or_typedef_name '{'  member_declaration_list '}'
		{
			tag_define($2.get_token(), $4);
			$$ = $4;
		}
        | aggregate_key identifier_or_typedef_name
		{ 
			Id const *id = tag_lookup($2.get_name());
			if (id) {
				unify(id->get_token(), $2.get_token());
				$$ = id->get_type();
				if (DP())
					cout << "lookup returns " << $$ << "\n";
			} else
				$$ = incomplete($2.get_token(), Block::get_scope_level());
		}
        ;

aggregate_key:
        STRUCT
        | UNION
        ;

member_declaration_list:
        member_declaration
        |  member_declaration_list member_declaration
		{
			if (DP()) {
				cout << "$1: " << $1 << "\n";
				cout << "$2: " << $2 << "\n";
			}
			// To avoid internal errors
			if ($1.is_valid() && $2.is_valid())
				$1.merge_with($2);
			$$ = $1;
		}
        ;

member_declaration:
        member_declaring_list ';'
		{ $$ = $1; }
        | member_default_declaring_list ';'
		{ $$ = $1; }
        ;

member_default_declaring_list:        /* doesn't redeclare typedef */
	/* volatile @ a[3] */
        type_qualifier_list member_identifier_declarator
		{
			if ($2.is_valid()) { // Check against padding bit fields
				$2.set_abstract($1);
				$$ = struct_union($2.get_token(), $2.type(), $1);
			} else
				$$ = struct_union($1);
		}
	/* volatile a[3], b */
        | member_default_declaring_list ',' member_identifier_declarator
		{
			if ($3.is_valid()) { // Check against padding bit fields
				$3.set_abstract($1.get_default_specifier());
				$1.add_member($3.get_token(), $3.type());
			}
			$$ = $1;
		}
        ;

member_declaring_list:
	/* unsigned int @ *a[3] */
        type_specifier member_declarator
		{
			if ($2.is_valid()) { // Check against padding bit fields
				$2.set_abstract($1);
				$$ = struct_union($2.get_token(), $2.type(), $1);
			} else
				$$ = struct_union($1);
			if (DP())
				cout << "(out)member_declaring_list = " << $$ << "\n";
		}
        | member_declaring_list ',' member_declarator
		{
			if ($3.is_valid()) { // Check against padding bit fields
				$3.set_abstract($1.get_default_specifier());
				$1.add_member($3.get_token(), $3.type());
			}
			$$ = $1;
		}
	/* struct {int hi, low;} - gcc/msc extension (anonymous structs) */
        | type_specifier
	{
		if (DP())
			cout << "anon member: " << $1 << "\n";
		if ($1.is_su()) {
			const Stab &s = $1.get_members();
			Stab_element::const_iterator i;
			
			for (i = s.begin(); i != s.end(); i++)
				if (i == s.begin())
					$$ = struct_union(
						(*i).second.get_token(),
						(*i).second.get_type(), $1);
				else
					$$.add_member(
						(*i).second.get_token(),
						(*i).second.get_type());
			if (DP())
				cout << "(out)member_declaring_list = " << $$ << "\n";
		} else {
			Error::error(E_ERR, "Only struct/union anonymous elements allowed");
			$$ = basic(b_undeclared);
		}
	}
        ;

member_declarator:
	/* *a[3] */
	/* a : 5 */
        declarator bit_field_size_opt
		{ $$ = $1; }
        | bit_field_size
		/* Padding bit field */
		{ $$ = basic(b_padbit); }
        ;

member_identifier_declarator:
	/* a[3]; also typedef names */
        identifier_declarator bit_field_size_opt
		{ $$ = $1; }
        | bit_field_size
		/* Padding bit field */
		{ $$ = basic(b_padbit); }
        ;

bit_field_size_opt:
        /* nothing */
        | bit_field_size
        ;

bit_field_size:
        ':' constant_expression
        ;

enum_name:
        ENUM '{' enumerator_list '}'
		{ $$ = enum_tag(); }
        | ENUM identifier_or_typedef_name '{' enumerator_list '}'
		{ tag_define($2.get_token(), $$ = enum_tag()); }
        | ENUM identifier_or_typedef_name
		{ 
			Id const *id = tag_lookup($2.get_name());
			if (id) {
				unify(id->get_token(), $2.get_token());
				$$ = id->get_type();
				if (DP())
					cout << "lookup returns " << $$ << "\n";
			} else
				$$ = basic(b_undeclared);
		}
        ;

enumerator_list:
        identifier_or_typedef_name enumerator_value_opt
			{ obj_define($1.get_token(), basic(b_int)); }
        | enumerator_list ',' identifier_or_typedef_name enumerator_value_opt
			{ obj_define($3.get_token(), basic(b_int)); }
        ;

enumerator_value_opt:
        /* Nothing */
        | '=' constant_expression
        ;

parameter_type_list:
        parameter_list
        | parameter_list ',' ELLIPSIS
        ;

parameter_list:
        parameter_declaration
        | parameter_list ',' parameter_declaration
        ;

parameter_declaration:
	/* int */
        declaration_specifier
	/* int [] */
        | declaration_specifier abstract_declarator
	/* int i[2] */
        | declaration_specifier identifier_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
	/* int FILE */
        | declaration_specifier parameter_typedef_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
	/* volatile */
        | declaration_qualifier_list
	/* volatile int */
        | declaration_qualifier_list abstract_declarator
	/* volatile int a */
        | declaration_qualifier_list identifier_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
	/* int */
        | type_specifier
        | type_specifier abstract_declarator
        | type_specifier identifier_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
        | type_specifier parameter_typedef_declarator
		{
			$2.set_abstract($1);
			$2.declare();
		}
        | type_qualifier_list
        | type_qualifier_list abstract_declarator
        | type_qualifier_list identifier_declarator
		{ $2.declare(); }
        ;

    /*  ANSI  C  section  3.7.1  states  "An identifier declared as a
    typedef name shall not be redeclared as a parameter".  Hence  the
    following is based only on IDENTIFIERs */

/* Only used for old-style function definitions, identifiers are declared
 * by default as int, but then a full declaration can follow. */
identifier_list:
        IDENTIFIER
			{ obj_define($1.get_token(), basic(b_int)); }
        | identifier_list ',' IDENTIFIER
			{ obj_define($3.get_token(), basic(b_int)); }
        ;

identifier_or_typedef_name:
        IDENTIFIER
        | TYPEDEF_NAME
        ;

type_name:
        type_specifier
        | type_specifier abstract_declarator
		{ $2.set_abstract($1); $$ = $2; }
        | type_qualifier_list
		{ $$ = basic(); }	/* We are ignoring qualifiers */
        | type_qualifier_list abstract_declarator
		{ $$ = $2; }
        ;

initializer_opt:
        /* nothing */
        | '=' initializer
        ;

initializer:
        '{' initializer_list '}'
        | '{' initializer_list ',' '}'
        | assignment_expression
        ;

initializer_list:
        initializer
        | initializer_list ',' initializer
        ;


/*************************** STATEMENTS *******************************/
statement: 
	any_statement
		{ Fchar::get_fileid().metrics().add_statement(); }
	;

any_statement:
        labeled_statement [YYVALID;]
        | compound_statement [YYVALID;]
        | expression_statement [YYVALID;]
        | selection_statement [YYVALID;]
        | iteration_statement [YYVALID;]
        | jump_statement [YYVALID;]
        ;

labeled_statement:
        identifier_or_typedef_name ':' statement
		{ label_define($1.get_token()); }
        | CASE constant_expression ':' statement
        | DEFAULT ':' statement
        ;

function_brace_begin: '{' 
		{ 
			Block::param_enter(); 
			Fchar::get_fileid().metrics().add_function();
		}
	;

brace_begin: '{' 
		{ Block::enter(); }
	;

brace_end: '}' 
		{ Block::exit(); }
	;

compound_statement:
        brace_begin brace_end
        | brace_begin declaration_list brace_end
        | brace_begin statement_list brace_end
        | brace_begin declaration_list statement_list brace_end
        ;

function_body:
        function_brace_begin brace_end
        | function_brace_begin declaration_list brace_end
        | function_brace_begin statement_list brace_end
        | function_brace_begin declaration_list statement_list brace_end
        ;

declaration_list:
        declaration
        | declaration_list declaration
        ;

statement_list:
        statement
        | statement_list statement
        ;

expression_statement:
        comma_expression_opt ';'
        ;

selection_statement:
          IF '(' comma_expression ')' statement
        | IF '(' comma_expression ')' statement ELSE statement
        | SWITCH '(' comma_expression ')' statement
        ;

iteration_statement:
        WHILE '(' comma_expression ')' statement
        | DO statement WHILE '(' comma_expression ')' ';'
        | FOR '(' comma_expression_opt ';' comma_expression_opt ';'
                comma_expression_opt ')' statement
        ;

jump_statement:
        GOTO identifier_or_typedef_name ';'
		{ label_use($2.get_token()); }
        | CONTINUE ';'
        | BREAK ';'
        | RETURN comma_expression_opt ';'
        ;


/***************************** EXTERNAL DEFINITIONS *****************************/

translation_unit:
        external_definition
        | translation_unit external_definition
        ;

external_definition:
        function_definition
			[ YYVALID; Function::exit(); Block::param_clear(); ]
        | declaration
			[ YYVALID; Block::param_clear(); ]
        ;

function_definition:
	/* foo(int a, int b) @ { } (and many illegal constructs) */
                                     identifier_declarator
		{ $1.declare(); }
					function_body
        | declaration_specifier      identifier_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | type_specifier             identifier_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | declaration_qualifier_list identifier_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | type_qualifier_list        identifier_declarator
		{ $2.declare(); }
					function_body

	/* foo(a, b) @ { } */
        |                            old_function_declarator
		{ $1.declare(); }
					function_body
        | declaration_specifier      old_function_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | type_specifier             old_function_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | declaration_qualifier_list old_function_declarator
		{ $2.set_abstract($1); $2.declare(); }
					function_body
        | type_qualifier_list        old_function_declarator
		{ $2.declare(); }
					function_body

	/* foo(a, b) @ int a; int b; @ { } */
        |                            old_function_declarator 
		{ Block::param_use(); } declaration_list
		{ Block::param_use_end(); $1.declare(); }
					function_body
        | declaration_specifier      old_function_declarator
		{ Block::param_use(); } declaration_list
		{ Block::param_use_end(); $2.set_abstract($1); $2.declare(); }
					function_body
        | type_specifier             old_function_declarator
		{ Block::param_use(); } declaration_list
		{ Block::param_use_end(); $2.set_abstract($1); $2.declare(); }
					function_body
        | declaration_qualifier_list old_function_declarator
		{ Block::param_use(); } declaration_list
		{ Block::param_use_end(); $2.set_abstract($1); $2.declare(); }
					function_body
        | type_qualifier_list        old_function_declarator
		{ Block::param_use(); } declaration_list
		{ Block::param_use_end(); $2.declare(); }
					function_body
        ;

declarator:
	/* *a[3] */
        identifier_declarator
        | typedef_declarator
        ; /* Default rules */

typedef_declarator:
        paren_typedef_declarator          /* would be ambiguous as parameter */
        | parameter_typedef_declarator    /* not ambiguous as parameter */
        ; /* Default rules */

parameter_typedef_declarator:
        TYPEDEF_NAME
        | TYPEDEF_NAME postfixing_abstract_declarator
		{ $1.set_abstract($2); $$ = $1; }
        | clean_typedef_declarator
        ;

    /*  The  following have at least one '*'. There is no (redundant)
    '(' between the '*' and the TYPEDEF_NAME. */

clean_typedef_declarator:
        clean_postfix_typedef_declarator
        | '*' parameter_typedef_declarator
		{ $2.set_abstract(pointer_to(basic())); $$ = $2; }
        | '*' type_qualifier_list parameter_typedef_declarator
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        ;

clean_postfix_typedef_declarator:
        '(' clean_typedef_declarator ')'
		{ $$ = $2; }
        | '(' clean_typedef_declarator ')' postfixing_abstract_declarator
		{ $2.set_abstract($4); $$ = $2; }
        ;

    /* The following have a redundant '(' placed immediately  to  the
    left of the TYPEDEF_NAME */

paren_typedef_declarator:
        paren_postfix_typedef_declarator
        | '*' '(' simple_paren_typedef_declarator ')' /* redundant paren */
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        | '*' type_qualifier_list
                '(' simple_paren_typedef_declarator ')' /* redundant paren */
		{ $4.set_abstract(pointer_to(basic())); $$ = $4; }
        | '*' paren_typedef_declarator
		{ $2.set_abstract(pointer_to(basic())); $$ = $2; }
        | '*' type_qualifier_list paren_typedef_declarator
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        ;

paren_postfix_typedef_declarator: /* redundant paren to left of tname*/
        '(' paren_typedef_declarator ')'
		{ $$ = $2; }
        | '(' simple_paren_typedef_declarator postfixing_abstract_declarator ')' /* redundant paren */
		{ $2.set_abstract($3); $$ = $2; }
        | '(' paren_typedef_declarator ')' postfixing_abstract_declarator
		{ $2.set_abstract($4); $$ = $2; }
        ;

simple_paren_typedef_declarator:
        TYPEDEF_NAME
        | '(' simple_paren_typedef_declarator ')'
		{ $$ = $2; }
        ;

identifier_declarator:
        unary_identifier_declarator
        | paren_identifier_declarator
        ; /* Default rules */

unary_identifier_declarator:
	/* a[3] */
        postfix_identifier_declarator
	/* *a[3] */
        | '*' identifier_declarator
		{ $2.set_abstract(pointer_to(basic())); $$ = $2; }
	/* * const a[3] */
        | '*' type_qualifier_list identifier_declarator
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        ;

postfix_identifier_declarator:
	/* int a[5]: declare a as array 5 of int */
        paren_identifier_declarator postfixing_abstract_declarator
		{ $1.set_abstract($2); $$ = $1; }
        | '(' unary_identifier_declarator ')'
		{ $$ = $2; }
	/*  int (*a)[10]: declare a as pointer to array 10 of int */
        | '(' unary_identifier_declarator ')' postfixing_abstract_declarator
		{ $2.set_abstract($4); $$ = $2; }
        ;

paren_identifier_declarator:
        IDENTIFIER
        | '(' paren_identifier_declarator ')'
		{ $$ = $2; }
        ;

old_function_declarator:
        postfix_old_function_declarator
        | '*' old_function_declarator
		{ $2.set_abstract(pointer_to(basic())); $$ = $2; }
        | '*' type_qualifier_list old_function_declarator
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        ;

postfix_old_function_declarator:
        paren_identifier_declarator '(' { Block::enter(); } identifier_list { Block::param_exit(); } ')'
		{ $1.set_abstract(function_returning(basic())); $$ = $1; }
        | '(' old_function_declarator ')'
		{ $$ = $2; }
        | '(' old_function_declarator ')' postfixing_abstract_declarator
		{ $2.set_abstract($4); $$ = $2; }
        ;

abstract_declarator:
        unary_abstract_declarator
        | postfix_abstract_declarator
        | postfixing_abstract_declarator
        ; /* Default rules */

postfixing_abstract_declarator:
        array_abstract_declarator
        | '(' ')'
		{ $$ = function_returning(basic()); }
        | '(' { Block::enter(); } parameter_type_list { Block::param_exit(); } ')'
		{ $$ = function_returning(basic()); }
        ;

array_abstract_declarator:
        '[' ']'
		{ $$ = array_of(basic()); }
        | '[' constant_expression ']'
		{ $$ = array_of(basic()); }
        | array_abstract_declarator '[' constant_expression ']'
		{ $$ = array_of($1); }
        ;

unary_abstract_declarator:
        '*'
		{ $$ = pointer_to(basic()); }
        | '*' type_qualifier_list
		{ $$ = pointer_to(basic()); }
        | '*' abstract_declarator
		{ $2.set_abstract(pointer_to(basic())); $$ = $2; }
        | '*' type_qualifier_list abstract_declarator
		{ $3.set_abstract(pointer_to(basic())); $$ = $3; }
        ;

postfix_abstract_declarator:
        '(' unary_abstract_declarator ')'
		{ $$ = $2; }
        | '(' postfix_abstract_declarator ')'
		{ $$ = $2; }
        | '(' postfixing_abstract_declarator ')'
		{ $$ = $2; }
        | '(' unary_abstract_declarator ')' postfixing_abstract_declarator
		{ $2.set_abstract($4); $$ = $2; }
        ;

%%
/* ----end of grammar----*/

    /* Copyright (C) 1989,1990 James A. Roskind, All rights reserved.
    This grammar was developed  and  written  by  James  A.  Roskind.
    Copying  of  this  grammar  description, as a whole, is permitted
    providing this notice is intact and applicable  in  all  complete
    copies.   Translations as a whole to other parser generator input
    languages  (or  grammar  description  languages)   is   permitted
    provided  that  this  notice is intact and applicable in all such
    copies,  along  with  a  disclaimer  that  the  contents  are   a
    translation.   The reproduction of derived text, such as modified
    versions of this grammar, or the output of parser generators,  is
    permitted,  provided  the  resulting  work includes the copyright
    notice "Portions Copyright (c)  1989,  1990  James  A.  Roskind".
    Derived products, such as compilers, translators, browsers, etc.,
    that  use  this  grammar,  must also provide the notice "Portions
    Copyright  (c)  1989,  1990  James  A.  Roskind"  in   a   manner
    appropriate  to  the  utility,  and in keeping with copyright law
    (e.g.: EITHER displayed when first invoked/executed; OR displayed
    continuously on display terminal; OR via placement in the  object
    code  in  form  readable in a printout, with or near the title of
    the work, or at the end of the file).  No royalties, licenses  or
    commissions  of  any  kind are required to copy this grammar, its
    translations, or derivative products, when the copies are made in
    compliance with this notice. Persons or corporations that do make
    copies in compliance with this notice may charge  whatever  price
    is  agreeable  to  a  buyer, for such copies or derivative works.
    THIS GRAMMAR IS PROVIDED ``AS IS'' AND  WITHOUT  ANY  EXPRESS  OR
    IMPLIED  WARRANTIES,  INCLUDING,  WITHOUT LIMITATION, THE IMPLIED
    WARRANTIES  OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR
    PURPOSE.

    James A. Roskind
    Independent Consultant
    516 Latania Palm Drive
    Indialantic FL, 32903
    (407)729-4348
    jar@ileaf.com


    ---end of copyright notice---


This file is a companion file to a C++ grammar description file.

*/


/* FILENAME: C.Y */

/*  This  is a grammar file for the dpANSI C language.  This file was
last modified by J. Roskind on 3/7/90. Version 1.00 */


/* ACKNOWLEDGMENT:

Without the effort expended by the ANSI C standardizing committee,  I
would  have been lost.  Although the ANSI C standard does not include
a fully disambiguated syntax description, the committee has at  least
provided most of the disambiguating rules in narratives.

Several  reviewers  have also recently critiqued this grammar, and/or
assisted in discussions during it's preparation.  These reviewers are
certainly not responsible for the errors I have committed  here,  but
they  are responsible for allowing me to provide fewer errors.  These
colleagues include: Bruce Blodgett, and Mark Langley. */

