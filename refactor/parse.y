/*
 * (C) Copyright 2001 Diomidis Spinellis.
 * Portions Copyright (c)  1989,  1990  James  A.  Roskind
 * Based on work by James A. Roskind; see comments at the end of this file.
 * Grammar obtained from http://www.empathy.com/pccts/roskind.html 
 *
 * Type inference engine.  Note that for the purposes of this work we do not
 * need to keep precise track of types, esp. implicit arithmetic conversions.
 * Type inference is used:
 * a) To identify the structure or union to use for member access
 * b) As a sanity check for (a)
 * c) To avoid mistages cause by ommitting part of the inference mechanism
 *
 * $Id: parse.y,v 1.10 2001/09/14 07:54:46 dds Exp $
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

#include "fileid.h"
#include "fileid.h"
#include "cpp.h"
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

void parse_error(char *s)
{
	Error::error(E_ERR, "syntax error");
}


#define YYSTYPE_CONSTRUCTOR

%}


%type <t> IDENTIFIER
%type <t> TYPEDEF_NAME
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
%type <t> type_name
%type <t> string_literal_list
%type <t> comma_expression

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
				Id i = $1.member($3.get_name());
				$$ = i.get_type();
				if ($$.is_valid()) {
					assert(i.get_name() == $3.get_name());
					unify($3.get_token(), i.get_token());
				}
			}
        | postfix_expression PTR_OP member_name
			{
				Id i = ($1.deref()).member($3.get_name());
				$$ = i.get_type();
				if ($$.is_valid()) {
					assert(i.get_name() == $3.get_name());
					unify($3.get_token(), i.get_token());
				}
			}
        | postfix_expression INC_OP
        | postfix_expression DEC_OP
        ;

member_name:
        IDENTIFIER
        | TYPEDEF_NAME
        ;

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
			{ $$ = $2; }
        ;

multiplicative_expression:
        cast_expression
        | multiplicative_expression '*' cast_expression
        | multiplicative_expression '/' cast_expression
        | multiplicative_expression '%' cast_expression
        ;

additive_expression:
        multiplicative_expression
        | additive_expression '+' multiplicative_expression
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
        ;

exclusive_or_expression:
        and_expression
        | exclusive_or_expression '^' and_expression
        ;

inclusive_or_expression:
        exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression
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
        | comma_expression ',' assignment_expression
			{ $$ = $3; }
        ;

constant_expression:
        conditional_expression
        ;

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
      struct S { T T;}; /* redefinition of T as member name * /

    Example of legal and illegal statements detected by this grammar:

      int; /* syntax error: vacuous declaration * /
      struct S;  /* no error: tag is defined or elaborated * /

    Example of result of proper declaration binding:

        int a=sizeof(a); /* note that "a" is declared with a type  in
            the name space BEFORE parsing the initializer * /

        int b, c[sizeof(b)]; /* Note that the first declarator "b" is
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
        declaration_qualifier_list identifier_declarator initializer_opt
        | type_qualifier_list identifier_declarator initializer_opt
        | default_declaring_list ',' identifier_declarator initializer_opt
        ;

declaring_list:
        declaration_specifier declarator initializer_opt
        | type_specifier declarator initializer_opt
        | declaring_list ',' declarator initializer_opt
        ;

declaration_specifier:
        basic_declaration_specifier          /* Arithmetic or void */
        | sue_declaration_specifier          /* struct/union/enum */
        | typedef_declaration_specifier      /* typedef*/
        ;

type_specifier:
        basic_type_specifier                 /* Arithmetic or void */
        | sue_type_specifier                 /* Struct/Union/Enum */
        | typedef_type_specifier             /* Typedef */
        ;


declaration_qualifier_list:  /* const/volatile, AND storage class */
        storage_class
        | type_qualifier_list storage_class
        | declaration_qualifier_list declaration_qualifier
        ;

type_qualifier_list:
        type_qualifier
        | type_qualifier_list type_qualifier
        ;

declaration_qualifier:
        storage_class
        | type_qualifier                  /* const or volatile */
        ;

type_qualifier:
        CONST
        | VOLATILE
        ;

basic_declaration_specifier:      /*Storage Class+Arithmetic or void*/
        declaration_qualifier_list basic_type_name
        | basic_type_specifier  storage_class
        | basic_declaration_specifier declaration_qualifier
        | basic_declaration_specifier basic_type_name
        ;

basic_type_specifier:
        basic_type_name            /* Arithmetic or void */
        | type_qualifier_list  basic_type_name
        | basic_type_specifier type_qualifier
        | basic_type_specifier basic_type_name
        ;

sue_declaration_specifier:          /* Storage Class + struct/union/enum */
        declaration_qualifier_list  elaborated_type_name
        | sue_type_specifier        storage_class
        | sue_declaration_specifier declaration_qualifier
        ;

sue_type_specifier:
        elaborated_type_name              /* struct/union/enum */
        | type_qualifier_list elaborated_type_name
        | sue_type_specifier  type_qualifier
        ;


typedef_declaration_specifier:       /*Storage Class + typedef types */
        typedef_type_specifier          storage_class
        | declaration_qualifier_list    TYPEDEF_NAME
        | typedef_declaration_specifier declaration_qualifier
        ;

typedef_type_specifier:              /* typedef types */
        TYPEDEF_NAME
        | type_qualifier_list    TYPEDEF_NAME
        | typedef_type_specifier type_qualifier
        ;

storage_class:
        TYPEDEF
        | EXTERN
        | STATIC
        | AUTO
        | REGISTER
        ;

basic_type_name:
        INT
        | CHAR
        | SHORT
        | LONG
        | FLOAT
        | DOUBLE
        | SIGNED
        | UNSIGNED
        | VOID
        ;

elaborated_type_name:
        aggregate_name
        | enum_name
        ;

aggregate_name:
        aggregate_key '{'  member_declaration_list '}'
        | aggregate_key identifier_or_typedef_name
                '{'  member_declaration_list '}'
        | aggregate_key identifier_or_typedef_name
        ;

aggregate_key:
        STRUCT
        | UNION
        ;

 member_declaration_list:
        member_declaration
        |  member_declaration_list member_declaration
        ;

member_declaration:
        member_declaring_list ';'
        | member_default_declaring_list ';'
        ;

member_default_declaring_list:        /* doesn't redeclare typedef*/
        type_qualifier_list member_identifier_declarator
        | member_default_declaring_list ',' member_identifier_declarator
        ;

member_declaring_list:
        type_specifier member_declarator
        | member_declaring_list ',' member_declarator
        ;


member_declarator:
        declarator bit_field_size_opt
        | bit_field_size
        ;

member_identifier_declarator:
        identifier_declarator bit_field_size_opt
        | bit_field_size
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
        | ENUM identifier_or_typedef_name '{' enumerator_list '}'
        | ENUM identifier_or_typedef_name
        ;



enumerator_list:
        identifier_or_typedef_name enumerator_value_opt
        | enumerator_list ',' identifier_or_typedef_name enumerator_value_opt
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
        declaration_specifier
        | declaration_specifier abstract_declarator
        | declaration_specifier identifier_declarator
        | declaration_specifier parameter_typedef_declarator
        | declaration_qualifier_list
        | declaration_qualifier_list abstract_declarator
        | declaration_qualifier_list identifier_declarator
        | type_specifier
        | type_specifier abstract_declarator
        | type_specifier identifier_declarator
        | type_specifier parameter_typedef_declarator
        | type_qualifier_list
        | type_qualifier_list abstract_declarator
        | type_qualifier_list identifier_declarator
        ;

    /*  ANSI  C  section  3.7.1  states  "An identifier declared as a
    typedef name shall not be redeclared as a parameter".  Hence  the
    following is based only on IDENTIFIERs */

/* Only used for old-style function definitions, identifiers are declared
 * by default as int, but they a full declaration can follow. */
identifier_list:
        IDENTIFIER
			{ obj_define($1.get_token(), basic(b_int)); }
        | identifier_list ',' IDENTIFIER
        ;

identifier_or_typedef_name:
        IDENTIFIER
        | TYPEDEF_NAME
        ;

type_name:
        type_specifier
        | type_specifier abstract_declarator
        | type_qualifier_list
        | type_qualifier_list abstract_declarator
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
        labeled_statement
        | compound_statement
        | expression_statement
        | selection_statement
        | iteration_statement
        | jump_statement
        ;

labeled_statement:
        identifier_or_typedef_name ':' statement
        | CASE constant_expression ':' statement
        | DEFAULT ':' statement
        ;

compound_statement:
        '{' '}'
        | '{' declaration_list '}'
        | '{' statement_list '}'
        | '{' declaration_list statement_list '}'
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
        | declaration
        ;

function_definition:
                                     identifier_declarator compound_statement
        | declaration_specifier      identifier_declarator compound_statement
        | type_specifier             identifier_declarator compound_statement
        | declaration_qualifier_list identifier_declarator compound_statement
        | type_qualifier_list        identifier_declarator compound_statement

        |                            old_function_declarator compound_statement
        | declaration_specifier      old_function_declarator compound_statement
        | type_specifier             old_function_declarator compound_statement
        | declaration_qualifier_list old_function_declarator compound_statement
        | type_qualifier_list        old_function_declarator compound_statement

        |                            old_function_declarator declaration_list
                compound_statement
        | declaration_specifier      old_function_declarator declaration_list
                compound_statement
        | type_specifier             old_function_declarator declaration_list
                compound_statement
        | declaration_qualifier_list old_function_declarator declaration_list
                compound_statement
        | type_qualifier_list        old_function_declarator declaration_list
                compound_statement
        ;

declarator:
        identifier_declarator
        | typedef_declarator
        ;

typedef_declarator:
        paren_typedef_declarator          /* would be ambiguous as parameter*/
        | parameter_typedef_declarator    /* not ambiguous as param*/
        ;

parameter_typedef_declarator:
        TYPEDEF_NAME
        | TYPEDEF_NAME postfixing_abstract_declarator
        | clean_typedef_declarator
        ;

    /*  The  following have at least one '*'. There is no (redundant)
    '(' between the '*' and the TYPEDEF_NAME. */

clean_typedef_declarator:
        clean_postfix_typedef_declarator
        | '*' parameter_typedef_declarator
        | '*' type_qualifier_list parameter_typedef_declarator
        ;

clean_postfix_typedef_declarator:
        '(' clean_typedef_declarator ')'
        | '(' clean_typedef_declarator ')' postfixing_abstract_declarator
        ;

    /* The following have a redundant '(' placed immediately  to  the
    left of the TYPEDEF_NAME */

paren_typedef_declarator:
        paren_postfix_typedef_declarator
        | '*' '(' simple_paren_typedef_declarator ')' /* redundant paren */
        | '*' type_qualifier_list
                '(' simple_paren_typedef_declarator ')' /* redundant paren */
        | '*' paren_typedef_declarator
        | '*' type_qualifier_list paren_typedef_declarator
        ;

paren_postfix_typedef_declarator: /* redundant paren to left of tname*/
        '(' paren_typedef_declarator ')'
        | '(' simple_paren_typedef_declarator postfixing_abstract_declarator ')' /* redundant paren */
        | '(' paren_typedef_declarator ')' postfixing_abstract_declarator
        ;

simple_paren_typedef_declarator:
        TYPEDEF_NAME
        | '(' simple_paren_typedef_declarator ')'
        ;

identifier_declarator:
        unary_identifier_declarator
        | paren_identifier_declarator
        ;

unary_identifier_declarator:
        postfix_identifier_declarator
        | '*' identifier_declarator
        | '*' type_qualifier_list identifier_declarator
        ;

postfix_identifier_declarator:
        paren_identifier_declarator postfixing_abstract_declarator
        | '(' unary_identifier_declarator ')'
        | '(' unary_identifier_declarator ')' postfixing_abstract_declarator
        ;

paren_identifier_declarator:
        IDENTIFIER
        | '(' paren_identifier_declarator ')'
        ;

old_function_declarator:
        postfix_old_function_declarator
        | '*' old_function_declarator
        | '*' type_qualifier_list old_function_declarator
        ;

postfix_old_function_declarator:
        paren_identifier_declarator '(' identifier_list ')'
        | '(' old_function_declarator ')'
        | '(' old_function_declarator ')' postfixing_abstract_declarator
        ;

abstract_declarator:
        unary_abstract_declarator
        | postfix_abstract_declarator
        | postfixing_abstract_declarator
        ;

postfixing_abstract_declarator:
        array_abstract_declarator
        | '(' ')'
        | '(' parameter_type_list ')'
        ;

array_abstract_declarator:
        '[' ']'
        | '[' constant_expression ']'
        | array_abstract_declarator '[' constant_expression ']'
        ;

unary_abstract_declarator:
        '*'
        | '*' type_qualifier_list
        | '*' abstract_declarator
        | '*' type_qualifier_list abstract_declarator
        ;

postfix_abstract_declarator:
        '(' unary_abstract_declarator ')'
        | '(' postfix_abstract_declarator ')'
        | '(' postfixing_abstract_declarator ')'
        | '(' unary_abstract_declarator ')' postfixing_abstract_declarator
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

