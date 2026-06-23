# Processing Yacc Files

Many C programs include parsing code in the form of *yacc* source files.
*CScout* can directly process those files, allowing you
to analyze and modify the identifiers used in those files.
*CScout* determines whether a file is *yacc* source or
plain C, by examining the file's suffix:
file names ending in a lowercase '`y`' are considered
to contain *yacc* source and processed accordingly.

*CScout* processes *yacc* files as follows:

-  All terminal and non-terminal names are considered to be
defined as file-scoped identifiers in a special *yacc*-only
namespace.
-  All terminal symbols are also marked as an undefined macro.
Thus, processing the *yacc*-generated file `y.tab.h`
immediately after the grammar (i.e. in the same scope) will
unify the terminal symbols with the corresponding macro definitions
throughout the program.
-  Members of the `%union` construct are defined as
members of the `YYSTYPE` `union` `typedef`.
These are then considered to be accessed in all
`%type` and the precedence specification constructs,
as well as the
explicit type specification through the `$<tag>#` construct.
-  No *yacc*-specific macros (`yyerrok`,
`YYABORT`,
`YYACCEPT`, etc)
are defined.
Feel free to define anything required to silence *CScout*
as a macro in the workspace definition file.

*CScout* is designed to process well-formed modern-style *yacc*
files.
All rules must be terminated with a semicolon
(apparently this is optional in the original *yacc* version).
The accepted grammar appears below.

```
body:
	defs '%%' rules tail
	;

tail:
	/* Empty */
	| '%%' c_code
	;

defs:
	/* Empty */
	| defs def
	;

def:
	'%start' IDENTIFIER
	| '%union' '{' member_declaration_list  '}' 
	| '%{' c_code '%}'
	| rword name_list_declaration
	;

rword:
	'%token'
	| '%left'
	| '%right'
	| '%nonassoc'
	| '%type'
	;

tag:
	/* Empty: union tag is optional */
	| '<' IDENTIFIER '>'
	;

name_list_declaration:
	tag name_number
	| name_list_declaration opt_comma name_number
	;

opt_comma:
	/* Empty */
	| ','
	;

name_number:
	name
	| name INT_CONST
	;

name:
	IDENTIFIER
	| CHAR_LITERAL
	;

/* rules section */

rules:
	rule
	| rules rule
	;

rule:
	IDENTIFIER ':'  rule_body_list ';'
	;

rule_body_list:
	rule_body
	| rule_body_list '|' rule_body
	;

rule_body:
	id_action_list prec
	;

id_action_list:
	/* Empty */
	| id_action_list name
        | id_action_list '{' c_code '}' 
	;

prec:
	/* Empty */
	| '%prec' name
	| '%prec' name  '{' c_code '}'
	;

variable:
	'$$'
	| '$' INT_CONST
	| '$-' INT_CONST
		{ $$ = basic(b_int); }
	| '$<' IDENTIFIER '>' variable_suffix
		{ $$ = $3; }
	;

variable_suffix:
	'$'
	| INT_CONST
	| '-' INT_CONST
	;
```
