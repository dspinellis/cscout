/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Based on a specification by Jutta Degener
 * (see older versions of the C grammar file)
 *
 * $Id: eval.y,v 1.19 2015/06/06 09:04:06 dds Exp $
 *
 */

/* Leave the space at the end of the following line! */
%include ytoken.h 

%start constant_expression

%{
#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <deque>
#include <vector>
#include <map>
#include <list>
#include <set>

#include "cpp.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"
#include "eval.h"

/*
 * From C99 6.3.1.8 Usual arithmetic conversions:
 * If both operands have the same type, then no further conversion is needed.
 * Otherwise, both operands are converted to the unsigned integer type
 * corresponding to the type of the operand with signed integer type.
 */
#define YYSTYPE struct s_ppval

// Unary and binary operators

// Preserve correct type, but don't act on it
#define unop(res, op, x) (res.v.u = (op x.v.u), res.su = x.su)
#define binop(res, x, op, y) (res.v.u = (x.v.u op y.v.u), (res.su = (x.su == y.su) ? x.su : e_unsigned))

// In relational binary operators signedness matters
// Result is 1 or 0, therefore always signed
#define relop(res, x, op, y) do {			\
	if (x.su == y.su) {				\
		if (x.su == e_signed)			\
			res.v.s = (x.v.s op y.v.s);	\
		else					\
			res.v.s = (x.v.u op y.v.u);	\
	} else						\
		res.v.s = (x.v.u op y.v.u);		\
	res.su = e_signed;				\
} while (0)

// Logical operations
// Result is 1 or 0, therefore always signed
#define logop(res, x, op, y) (res.v.u = (x.v.u op y.v.u), res.su = e_signed)

void eval_error(const char *, ...) {}
extern int eval_lex();

%}

%%

primary_expression
        : INT_CONST
        | '(' expression ')'	{ $$ = $2; }
        ;

postfix_expression
        : primary_expression
        ;

unary_expression
        : postfix_expression
        | '+' cast_expression	{ $$ = $2; }
        | '-' cast_expression	{ unop($$, -, $2); }
        | '~' cast_expression	{ unop($$, ~, $2); }
        | '!' cast_expression	{ unop($$, !, $2); }
        ;

cast_expression
        : unary_expression
        ;

multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression	{ binop($$, $1, *, $3); }
        | multiplicative_expression '/' cast_expression
			{
				if ($3.v.u == 0) {
					/*
					 * @error
					 * A <code>#if</code> expression
					 * divided by zero
					 */
					Error::error(E_ERR, "division by zero in #if expression");
					$$.v.u = 0;
				} else
					binop($$, $1, /, $3);
			}
        | multiplicative_expression '%' cast_expression
			{
				if ($3.v.u == 0) {
					/*
					 * @error
					 * A <code>#if</code> expression
					 * divided by zero in a modulo
					 * expression
					 */
					Error::error(E_ERR, "modulo division by zero in #if expression");
					$$ = $1;
				} else
					binop($$, $1, %, $3);
			}
        ;

additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression { binop($$, $1, +, $3); }
        | additive_expression '-' multiplicative_expression { binop($$, $1, -, $3); }
        ;

shift_expression
        : additive_expression
        | shift_expression LEFT_OP additive_expression	{ binop($$, $1, <<, $3); }
        | shift_expression RIGHT_OP additive_expression
			{
				$$.su = (($1.su == $2.su) ? $1.su : e_unsigned);
				if ($1.su == e_signed)
					$$.v.s = ($1.v.s >> $3.v.u);
				else
					$$.v.u = ($1.v.u >> $3.v.u);
			}
        ;

relational_expression
        : shift_expression
        | relational_expression '<' shift_expression	{ relop($$, $1, <, $3); }
        | relational_expression '>' shift_expression	{ relop($$, $1, >, $3); }
        | relational_expression LE_OP shift_expression	{ relop($$, $1, <=, $3); }
        | relational_expression GE_OP shift_expression	{ relop($$, $1, >=, $3); }
        ;

equality_expression
        : relational_expression
        | equality_expression EQ_OP relational_expression { relop($$, $1, ==, $3); }
        | equality_expression NE_OP relational_expression { relop($$, $1, !=, $3); }
        ;

and_expression
        : equality_expression
        | and_expression '&' equality_expression	{ binop($$, $1, &, $3); }
        ;

exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression	{ binop($$, $1, ^, $3); }
        ;

inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression	{ binop($$, $1, |, $3); }
        ;

logical_and_expression
        : inclusive_or_expression
        | logical_and_expression AND_OP inclusive_or_expression
							{ logop($$, $1, &&, $3); }
        ;

logical_or_expression
        : logical_and_expression
        | logical_or_expression OR_OP logical_and_expression { logop($$, $1, ||, $3); }
        ;

conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
			{
				$$.su = (($3.su == $5.su) ? $3.su : e_unsigned);
				$$.v.u = $1.v.u ? $3.v.u : $5.v.u;
			}
        ;

constant_expression
        : conditional_expression
		{
			extern long eval_result;

			eval_result = $1.v.u;
		}
        ;

expression: constant_expression
	;
%%
