/*
 * (C) Copyright 2001 Diomidis Spinellis.
 * Based on an ANSI yacc grammar by Jutta Degener (see below)
 *
 * Type inference engine.  Note that for the purposes of this work we do not
 * need to keep precise track of types, esp. implicit arithmetic conversions.
 * Type inference is used:
 * a) To identify the structure or union to use for member access
 * b) As a sanity check for (a)
 *
 *
 * $Id: parse.y,v 1.3 2001/09/04 13:40:30 dds Exp $
 *
 */

/*
ANSI C Yacc grammar

In 1985, Jeff Lee published his Yacc grammar (which is accompanied by a matching Lex specification) for the April 30, 1985 draft version of the ANSI C
standard.  Tom Stockfisch reposted it to net.sources in 1987; that original, as mentioned in the answer to question 17.25 of the comp.lang.c FAQ, can be
ftp'ed from ftp.uu.net, file usenet/net.sources/ansi.c.grammar.Z. 

I intend to keep this version as close to the current C Standard grammar as possible; please let me know if you discover discrepancies. 

Jutta Degener, 1995 
*/


%token IDENTIFIER INT_CONST FLOAT_CONST STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

/* Tokens needed by the pre-processor */
%token CPP_CONCAT SPACE ABSFNAME PATHFNAME CHAR_LITERAL PP_NUMBER

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

#include "fileid.h"
#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "error.h"
#include "id.h"
#include "type.h"

void yyerror(char *s) {}
int yylex(void) { return 0;}


%}


%union {
	Type *t;
	Id *i;
};


%type <i> IDENTIFIER

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
%type <t> expression
%type <t> constant_expression
%type <t> assignment_expression
%type <t> type_name

%%

primary_expression
        : IDENTIFIER
			{ $$ = $1->get_type(); }
        | INT_CONST
			{ $$ = new Tbasic(b_int); }
        | FLOAT_CONST
			{ $$ = new Tbasic(b_float); }
        | STRING_LITERAL
			{ $$ = new Tarray(new Tbasic(b_char)); }
        | '(' expression ')'	
			{ $$ = $2; }
        ;

postfix_expression
        : primary_expression
        | postfix_expression '[' expression ']'
			{ 
				$$ = $1->subscript(); 
				delete $1; 
				delete $3; 
			}
        | postfix_expression '(' ')'
			{ $$ = $1->call(); delete $1; }
        | postfix_expression '(' argument_expression_list ')'
			{ $$ = $1->call(); delete $1; }
        | postfix_expression '.' IDENTIFIER
			{
				Id *i = $1->member($3->get_name());
				$$ = i->get_type();
				assert(i->get_name() == $3->get_name());
				unify($3->get_token(), i->get_token());
				delete $1;
				delete $3;
			}
        | postfix_expression PTR_OP IDENTIFIER
			{
				Id *i = ($1->deref())->member($3->get_name());
				$$ = i->get_type();
				assert(i->get_name() == $3->get_name());
				unify($3->get_token(), i->get_token());
				delete $1;
				delete $3;
			}
        | postfix_expression INC_OP
        | postfix_expression DEC_OP
        ;

argument_expression_list
        : assignment_expression
			{ delete $1; }
        | argument_expression_list ',' assignment_expression
			{ delete $3; }
        ;

unary_expression
        : postfix_expression
        | INC_OP unary_expression
			{ $$ = $2; }
        | DEC_OP unary_expression
			{ $$ = $2; }
        | arith_unary_operator cast_expression
			{ $$ = $2; }
        | '&' cast_expression
			{ $$ = new Tptr($2); }
        | '*' cast_expression
			{ $$ = $2->deref(); delete $2; }
        | SIZEOF unary_expression
			{ $$ = new Tbasic(b_int); delete $2; }
        | SIZEOF '(' type_name ')'
			{ $$ = new Tbasic(b_int); delete $3; }
        ;

arith_unary_operator
        : '+'
        | '-'
        | '~'
        | '!'
        ;

cast_expression
        : unary_expression
        | '(' type_name ')' cast_expression
			{ $$ = $2;  delete $4; }
        ;

multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression
			{ $$ = $1; delete $3; }
        | multiplicative_expression '/' cast_expression
			{ $$ = $1; delete $3; }
        | multiplicative_expression '%' cast_expression
			{ $$ = $1; delete $3; }
        ;

additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression
			{ $$ = $1; delete $3; }
        | additive_expression '-' multiplicative_expression
			{
				if ($1->is_ptr() && $3->is_ptr()) {
					$$ = new Tbasic(b_int);
					delete $1;
				} else 
					$$ = $1;
				delete $3;
			}
        ;

shift_expression
        : additive_expression
        | shift_expression LEFT_OP additive_expression
			{ $$ = $1; delete $3; }
        | shift_expression RIGHT_OP additive_expression
			{ $$ = $1; delete $3; }
        ;

relational_expression
        : shift_expression
        | relational_expression '<' shift_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        | relational_expression '>' shift_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        | relational_expression LE_OP shift_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        | relational_expression GE_OP shift_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        ;

equality_expression
        : relational_expression
        | equality_expression EQ_OP relational_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        | equality_expression NE_OP relational_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        ;

and_expression
        : equality_expression
        | and_expression '&' equality_expression
			{ $$ = $1; delete $3; }
        ;

exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression
			{ $$ = $1; delete $3; }
        ;

inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression
			{ $$ = $1; delete $3; }
        ;

logical_and_expression
        : inclusive_or_expression
        | logical_and_expression AND_OP inclusive_or_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        ;

logical_or_expression
        : logical_and_expression
        | logical_or_expression OR_OP logical_and_expression
			{ $$ = new Tbasic(b_int); delete $1; delete $3; }
        ;

conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
			{ $$ = $3; delete $1; delete $5; }
        ;

assignment_expression
        : conditional_expression
        | unary_expression assignment_operator assignment_expression
			{ $$ = $1; delete $3; }
        ;

assignment_operator
        : '='
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

expression
        : assignment_expression
        | expression ',' assignment_expression
			{ $$ = $3; delete $1; }
        ;

constant_expression
        : conditional_expression
        ;

declaration
        : declaration_specifiers ';'
        | declaration_specifiers init_declarator_list ';'
        ;

declaration_specifiers
        : storage_class_specifier
        | storage_class_specifier declaration_specifiers
        | type_specifier
        | type_specifier declaration_specifiers
        | type_qualifier
        | type_qualifier declaration_specifiers
        ;

init_declarator_list
        : init_declarator
        | init_declarator_list ',' init_declarator
        ;

init_declarator
        : declarator
        | declarator '=' initializer
        ;

storage_class_specifier
        : TYPEDEF
        | EXTERN
        | STATIC
        | AUTO
        | REGISTER
        ;

type_specifier
        : VOID
        | CHAR
        | SHORT
        | INT
        | LONG
        | FLOAT
        | DOUBLE
        | SIGNED
        | UNSIGNED
        | struct_or_union_specifier
        | enum_specifier
        | TYPE_NAME
        ;

struct_or_union_specifier
        : struct_or_union IDENTIFIER '{' struct_declaration_list '}'
        | struct_or_union '{' struct_declaration_list '}'
        | struct_or_union IDENTIFIER
        ;

struct_or_union
        : STRUCT
        | UNION
        ;

struct_declaration_list
        : struct_declaration
        | struct_declaration_list struct_declaration
        ;

struct_declaration
        : specifier_qualifier_list struct_declarator_list ';'
        ;

specifier_qualifier_list
        : type_specifier specifier_qualifier_list
        | type_specifier
        | type_qualifier specifier_qualifier_list
        | type_qualifier
        ;

struct_declarator_list
        : struct_declarator
        | struct_declarator_list ',' struct_declarator
        ;

struct_declarator
        : declarator
        | ':' constant_expression
        | declarator ':' constant_expression
        ;

enum_specifier
        : ENUM '{' enumerator_list '}'
        | ENUM IDENTIFIER '{' enumerator_list '}'
        | ENUM IDENTIFIER
        ;

enumerator_list
        : enumerator
        | enumerator_list ',' enumerator
        ;

enumerator
        : IDENTIFIER
        | IDENTIFIER '=' constant_expression
        ;

type_qualifier
        : CONST
        | VOLATILE
        ;

declarator
        : pointer direct_declarator
        | direct_declarator
        ;

direct_declarator
        : IDENTIFIER
        | '(' declarator ')'
        | direct_declarator '[' constant_expression ']'
        | direct_declarator '[' ']'
        | direct_declarator '(' parameter_type_list ')'
        | direct_declarator '(' identifier_list ')'
        | direct_declarator '(' ')'
        ;

pointer
        : '*'
        | '*' type_qualifier_list
        | '*' pointer
        | '*' type_qualifier_list pointer
        ;

type_qualifier_list
        : type_qualifier
        | type_qualifier_list type_qualifier
        ;


parameter_type_list
        : parameter_list
        | parameter_list ',' ELLIPSIS
        ;

parameter_list
        : parameter_declaration
        | parameter_list ',' parameter_declaration
        ;

parameter_declaration
        : declaration_specifiers declarator
        | declaration_specifiers abstract_declarator
        | declaration_specifiers
        ;

identifier_list
        : IDENTIFIER
        | identifier_list ',' IDENTIFIER
        ;

type_name
        : specifier_qualifier_list
        | specifier_qualifier_list abstract_declarator
        ;

abstract_declarator
        : pointer
        | direct_abstract_declarator
        | pointer direct_abstract_declarator
        ;

direct_abstract_declarator
        : '(' abstract_declarator ')'
        | '[' ']'
        | '[' constant_expression ']'
        | direct_abstract_declarator '[' ']'
        | direct_abstract_declarator '[' constant_expression ']'
        | '(' ')'
        | '(' parameter_type_list ')'
        | direct_abstract_declarator '(' ')'
        | direct_abstract_declarator '(' parameter_type_list ')'
        ;

initializer
        : assignment_expression
        | '{' initializer_list '}'
        | '{' initializer_list ',' '}'
        ;

initializer_list
        : initializer
        | initializer_list ',' initializer
        ;

statement
        : labeled_statement
        | compound_statement
        | expression_statement
        | selection_statement
        | iteration_statement
        | jump_statement
        ;

labeled_statement
        : IDENTIFIER ':' statement
        | CASE constant_expression ':' statement
        | DEFAULT ':' statement
        ;

compound_statement
        : '{' '}'
        | '{' statement_list '}'
        | '{' declaration_list '}'
        | '{' declaration_list statement_list '}'
        ;

declaration_list
        : declaration
        | declaration_list declaration
        ;

statement_list
        : statement
        | statement_list statement
        ;

expression_statement
        : ';'
        | expression ';'
        ;

selection_statement
        : IF '(' expression ')' statement
        | IF '(' expression ')' statement ELSE statement
        | SWITCH '(' expression ')' statement
        ;

iteration_statement
        : WHILE '(' expression ')' statement
        | DO statement WHILE '(' expression ')' ';'
        | FOR '(' expression_statement expression_statement ')' statement
        | FOR '(' expression_statement expression_statement expression ')' statement
        ;

jump_statement
        : GOTO IDENTIFIER ';'
        | CONTINUE ';'
        | BREAK ';'
        | RETURN ';'
        | RETURN expression ';'
        ;

translation_unit
        : external_declaration
        | translation_unit external_declaration
        ;

external_declaration
        : function_definition
        | declaration
        ;

function_definition
        : declaration_specifiers declarator declaration_list compound_statement
        | declaration_specifiers declarator compound_statement
        | declarator declaration_list compound_statement
        | declarator compound_statement
        ;

%%
