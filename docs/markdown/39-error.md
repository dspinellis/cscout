# Error Messages

## Warnings

-  `#ifdef argument is not an identifier`  

The token following a
`#ifdef`
directive is not a legal identifier.

-  `Application of macro " ... ": operator # only valid before macro parameters`  

The stringizing operator
`#` was not followed
by a macro parameter.

-  `Assuming declaration int  ... (...)`  

An undeclared identifier is used as a function.

-  `Duplicate (different) macro definition of macro  ...`  

A defined macro can be redefined only if the
two definitions are exactly the same.

-  `Empty character literal`  

Character lirerals must include a character.

-  `Illegal combination of sign specifiers`  

The signedness specifiers used can not be combined
(e.g. `unsigned signed`).

-  `Processing automatically generated file; #line directive ignored.`  

A
`#line`
directive was found.
This signifies that the source file is automatically
generated and refactoring changes on that file may
be lost in the future.
References to the original source file are ignored.

-  `Sign specification on non-integral type - ignored`  

A signedness specification was given on a non-integral type.

-  `Undeclared identifier in typeof expression:  ...`  

The identifier appearing within
`typeof`
has not been declared.

-  `Undeclared identifier:  ...`  

An undeclared identifier was used
in a primary expression.

## Fatal Errors

-  `#pragma pushd: unable to get current directory:  ...`  

The call to `getcwd` failed while
processing the 
`#pragma pushd`
CScout-specific directive.

-  `EOF in comment`  

The end of file was reached while 
processing a block comment.

-  `Error count exceeds 100; exiting ...`  

To avoid cascading errors only the first 100 errors
are reported.

-  `Invalid C token: '$'`  

A '$' token was encountered in C code.
Values starting with a '$' token are only allowed inside
yacc rules.

-  `Unable to get path of file  ...`  

The Win32 GetfullPathName system call used to retrieve the
unique path file name failed (Windows-specific).

-  `Unable to stat file  ...`  

The POSIX stat system call used to retrieve the
unique file identifier failed (Unix-specific).

## Errors

-  `#pragma echo: string expected`  

The
`#pragma echo`
CScout-specific directive was not followed by a 
string.

-  `#pragma includepath: string expected`  

The
`#pragma includepath`
CScout-specific directive was not followed by a 
string.

-  `#pragma process: string expected`  

The
`#pragma process`
CScout-specific directive was not followed by a 
string.

-  `#pragma project: string expected`  

The
`#pragma project`
CScout-specific directive was not followed by a 
string.

-  `#pragma pushd: string expected`  

The
`#pragma pushd`
CScout-specific directive was not followed by a 
string.

-  `#pragma readonly: string expected`  

The
`#pragma readonly`
CScout-specific directive was not followed by a 
string.

-  `#pragma ro_prefix: string expected`  

The
`#pragma ro_prefix`
CScout-specific directive was not followed by a 
string.

-  `% not followed by yacc keyword`  

In the definitions section of a yacc file the
% symbol was not followed by a legal yacc keyword.

-  `%union does not have a member  ...`  

The member used in a $<name>X yacc construct
was not defined as a %union member.

-  `Application of macro " ... ": operator # at end of macro pattern`  

No argument was supplied to the right
of the stringizing operator 
`#`.

-  `Array not an abstract type`  

The underlying array object for which a type is
specified is not an abstract type.

-  `At most one storage class can be specified`  

More than one storage class was given for the same
object.

-  `Conflicting declarations for identifier  ...`  

An identifier is declared twice
with compilation or linkage
unit scope with conflicting
declarations.

-  `Declared parameter does not appear in old-style function parameter list:  ...`  

While processing an old-style (K&R) parameter
declaration, a declared parameter did not match
any of the parameters appearing in the function's
arguments definition.

-  `Division by zero in #if expression`  

A `#if` expression
divided by zero.

-  `Duplicate definition of identifier  ...`  

An identifier is declared twice within the
same block.

-  `Duplicate definition of tag   ...`  

A structure, union, or enumeration tag was defined
twice for the same entity.

-  `EOF while processing #if directive`  

The processing of code within a `#if`
block reached the end of file, without a
corresponding
`#endif` /
`#else` /
`#elif` directive.

-  `Empty #include directive`  

A
`#include`
directive was not followed by a filename specification.

-  `End of file in character literal`  

The end of file was reached while 
processing a character literal:
a single quote was never closed.

-  `End of file in string literal`  

The end of file was reached while 
processing a string.

-  `Explicit element tag without no %union in effect`  

The yacc $n syntax was used
to specify an element of the %union 
but no union was defined.

-  `Illegal characters in hex escape sequence`  

A hexadecimal character escape sequence
`\x` continued with a non-hexadecimal 
character.

-  `Illegal combination of type specifiers`  

The type specifiers used can not be combined
(e.g. `double char`).

-  `Illegal pointer dereference`  

An attempt was made to dereference an element that is not a pointer.

-  `Invalid #include syntax`  

A
`#include`
directive was not followed by a legal filename specification.

-  `Invalid application of basic type, storage class, or type specifier`  

A basic type, storage class or type specified was specified in
an invalid underlying object.

-  `Invalid character escape sequence`  

A character escape sequence `\c` can not be
recognised.

-  `Invalid macro name`  

The macro name specified in a 
`#define` or
`#undef`
directive is not a valid identifier.

-  `Invalid macro parameter name`  

A macro parameter name specified in a 
`#define`
directive is not a valid identifier.

-  `Invalid macro parameter punctuation`  

The formal parameters in a
`#define` macro
definition are not separated
by commas.

-  `Invalid preprocessor directive`  

An invalid preprocessor directive was found.
The directive did not match an identifier.

-  `Invalid type specification`  

An attempt was made to specify a type on an object that
did not allow this specification.

-  `Label  ...  already defined`  

The same `goto` label is defined more 
than once in a given function.

-  `Macro [ ... ]: EOF while reading function macro arguments`  

The end of file was reached while 
gathering a macro's arguments.

-  `Macro [ ... ]: close bracket expected for function-like macro`  

The arguments to a function-like macro did
not terminate with a closing bracket.

-  `Member access in incomplete struct/union:  ...`  

The member access for a structure or union is applied
on an object with an incomplete definition.

-  `Missing close bracket in defined operator`  

The identifier of a 
`defined` operator was not
followed by a closing bracket.

-  `Modulo division by zero in #if expression`  

A `#if` expression
divided by zero in a modulo 
expression.

-  `Multiple storage classes in type declaration`  

Incompatible storage classes were specified in a single
type declaration.

-  `No identifier following defined operator`  

The 
`defined` operator was not followed
by an identifier.

-  `Object is not a function`  

The object used as a function to be called is not a function
or a pointer to a function.

-  `Only struct/union anonymous elements allowed`  

Anonymous members within a member
declaring list (e.g.
`struct {int x, y;}`)
can only be structures or unions.
(GCC/Microsoft C extension).

-  `Pointer not an abstract type`  

The underlying pointer object for which a type is
specified is not an abstract type.

-  `Popd: directory stack empty`  

The
`#pragma popd`
CScout-specific directive was performed on an
empty directory stack.

-  `Structure or union does not have a member  ...`  

The structure or union on the left
of the
`.` or
`->` operator
does not have as a member the
identifier appearing on the
operator's right.

-  `Subscript not on array or pointer`  

The object being subscripted using the `[]`
operator is not an array or a pointer.

-  `Syntax error in preprocessor expression`  

A
`#if` or
`#elif` expression was syntactically
incorrect.

-  `Unable to open include file  ...`  

The specified include file could not be opened.

-  `Unbalanced #elif`  

A
`#elif`
directive was found without a corresponding
`#if`.

-  `Unbalanced #else`  

A
`#else`
directive was found without a corresponding
`#if`.

-  `Unbalanced #endif`  

A
`#endif`
directive was found without a corresponding
`#if`.

-  `Undefined label  ...`  

A `goto`
label used within a function was never defined.

-  `Unexpected end in character escape sequence`  

A character escape sequence (`\c`) was not
completed; no character follows the backslash.

-  `Unknown preprocessor directive:  ...`  

An unkown preprocessor directive was found.

-  `Unkown %union element tag  ...`  

The yacc %union 
does not have as a member the
identifier appearing on the
element's tag.

-  `Yacc $value out of range`  

The number used in a $n yacc variable was greater than the
number of identifiers and actions on the rule's left side.
