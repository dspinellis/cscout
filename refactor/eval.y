/*
 * (C) Copyright 2001 Diomidis Spinellis.
 * Based on a specification by Jutta Degener
 * (see older versions of the C grammar file)
 *
 * $Id: eval.y,v 1.7 2002/09/17 07:55:39 dds Exp $
 *
 */

%include ytoken.h

%start constant_expression

%{
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stack>
#include <deque>
#include <vector>
#include <map>
#include <list>

#include "cpp.h"
#include "metrics.h"
#include "fileid.h"
#include "attr.h"
#include "tokid.h"
#include "fchar.h"
#include "error.h"

#define YYSTYPE long

void eval_error(char *) {}
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
        | '-' cast_expression	{ $$ = -$2; }
        | '~' cast_expression	{ $$ = ~$2; }
        | '!' cast_expression	{ $$ = !$2; }
        ;

cast_expression
        : unary_expression
        ;

multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression	{ $$ = $1 * $3; }
        | multiplicative_expression '/' cast_expression	
			{
				if ($3 == 0) {
					Error::error(E_ERR, "division by zero in #if expression");
					$$ = 0;
				} else
					$$ = $1 / $3;
			}
        | multiplicative_expression '%' cast_expression
			{
				if ($3 == 0) {
					Error::error(E_ERR, "modulo division by zero in #if expression");
					$$ = $1;
				} else
					$$ = $1 / $3;
			}
        ;

additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression { $$ = $1 + $3; }
        | additive_expression '-' multiplicative_expression { $$ = $1 - $3; }
        ;

shift_expression
        : additive_expression
        | shift_expression LEFT_OP additive_expression	{ $$ = $1 << $3; }
        | shift_expression RIGHT_OP additive_expression	{ $$ = $1 >> $3; }
        ;

relational_expression
        : shift_expression
        | relational_expression '<' shift_expression	{ $$ = $1 < $3; }
        | relational_expression '>' shift_expression	{ $$ = $1 > $3; }
        | relational_expression LE_OP shift_expression	{ $$ = $1 <= $3; }
        | relational_expression GE_OP shift_expression	{ $$ = $1 >= $3; }
        ;

equality_expression
        : relational_expression
        | equality_expression EQ_OP relational_expression { $$ = ($1 == $3); }
        | equality_expression NE_OP relational_expression { $$ = ($1 != $3); }
        ;

and_expression
        : equality_expression
        | and_expression '&' equality_expression	{ $$ = ($1 & $3); }
        ;

exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression	{ $$ = ($1 ^ $3); }
        ;

inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression	{ $$ = ($1 | $3); }
        ;

logical_and_expression
        : inclusive_or_expression
        | logical_and_expression AND_OP inclusive_or_expression	
							{ $$ = ($1 & $3); }
        ;

logical_or_expression
        : logical_and_expression
        | logical_or_expression OR_OP logical_and_expression { $$ = ($1 | $3); }
        ;

conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression
						{ $$ = ($1 ? $3 : $5); }
        ;

constant_expression
        : conditional_expression
		{
			extern long eval_result;

			eval_result = $1;
		}
        ;

expression: constant_expression
	;
%%
