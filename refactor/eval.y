/*
 * (C) Copyright 2001 Diomidis Spinellis.
 * Based on a specification by Jutta Degener (see C grammar file)
 *
 * $Id: eval.y,v 1.1 2001/09/02 09:23:45 dds Exp $
 *
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

%start constant_expression

%{
#define YYSTYPE long

void eval_error(char *){}
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
        | multiplicative_expression '/' cast_expression	{ $$ = $1 / $3; }
        | multiplicative_expression '%' cast_expression	{ $$ = $1 % $3; }
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
