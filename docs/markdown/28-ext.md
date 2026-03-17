# Language Extensions

*CScout*  implements the
C language as defined in ANSI X3.159-1989.
In addition, it supports the following extensions.
Many GCC extensions also apply to LLVM.

1.  Preprocessor directives can appear within a call to a function-like macro (GCC)

1.  Initializers and compound literals can be empty (GCC)

1.  The `alignof` operator can be used on types (GCC) 

1.  A declaration expression as the first element of a `for` statement (C99)
1.  The `restrict` qualifier and the `inline` specifier (C99)
1.  The `_Bool` type (C99)
1.  The `_Thread_local` storage class specifier (C11)
1.  The `_Generic` generic selection expression (C11)
1.  Support `static` and other qualifiers in array type declarations (C99)
1.  Initialization designators (C99)
1.  Array initialization designators can include ranges (GCC) 

1.  ANSI-style function definitions can be nested (GCC) (GCC also allows
nested K&R-style function definitions)

1.  The equals sign following an initializer designator is optional (GCC) 

1.  Array and structure initialization (GCC)
1.  Compound literals (C99)
1.  Declarations can be intermixed with statements (C99).
1.  Recognise `__atribute__(__unused__)` for determining which
identifiers should not be reported as unused (GCC).
1.  `//` line comments (common extension)
1.  `__asm__` blocks (GCC)
1.  `enum` lists ending with a comma (common extension)
1.  Anonymous `struct/union` members (GCC, Microsoft C)
1.  Allow `case` expression ranges (GCC).
1.  An enumeration list can be empty (Microsoft C)
1. Allow braces around scalar initializers (common extension).

1.  Indirect `goto` targets and the label address-of operator (GCC).
1.  `__typeof` keyword (GCC)
1.  `__typeof__unqual__` keyword (GCC)
1.  The ‘##’ token paste operator deletes the comma when placed between a comma and an empty variable argument (GCC)
1.  `__VA_OPT__ ` function macro (C23)
1.  `auto` (C23) and `__auto_type` (GCC)
1.  A compound statement in brackets can be an expression (GCC)
1.  Macros expanding from `/##/` into
`//` are then treated as a line comment (Microsoft C)
1.  Exception handling using the `__try __except __finally __leave` keywords (Microsoft C) 

1.  `#include_next` preprocessor directive (GCC)
1.  `#warning` preprocessor directive (GCC)
1.  Variable number of arguments preprocessor macros
(support for both the GCC and the C99 syntax)
1.  Allow empty member declarations in aggregates (GCC)
1.  `long long` type (common extension)
1.  A semicolon can appear as a declatation (common extension)
1.  An aggregate declaration body can be empty (GCC)
1.  Aggregate member initialization using the member: value syntax (GCC)
1.  Statement labels do not require a statement following them (GCC)
1.  #ident preprocessor directive (GCC)
1.  Allow assignment to case expressions (common extension)
1.  Accept an empty translation unit (common extension).
1.  Support locally declared labels (`__label__`) (GCC).
1.  The second argument of a conditional expression can be omitted (GCC).
1.  Dereferencing a function yields a function (common extension).
1.  The `__simd` type qualifier can be used to define
     basic types that are also indexable, similarly to the GCC
     extension `__attribute__((__vector_size__(…)))`.

Many other compiler-specific extensions are handled by suitable
macro definitions in the *CScout* initialization file.
