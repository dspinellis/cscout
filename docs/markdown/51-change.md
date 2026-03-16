# Change History

**Version 3.1 (under development)**: 

**Version 3.0 (2016-01-17)**: 
	

		- Source code, repository, build process, and installation procedures were polished for distribution and released through GitHub licensed under GPL.

		- Changes and features are from now on tracked through the publicly available Git commit log.

	

**Version 2.8 (2011-10-23)**: 

-  New `-C` option will create a *ctags* compatible
tags file, which many editors can use to navigate through the code.

-  New `-d D` option will dump on the standard output
the `#define` directives that get processed.

-  New `-d H` option will dump on the standard output
the (mostly header) files getting included.

-  Correct appearance of page labels whene these span more than one line.

-  Functions defined through macro calls get a more precise location of
the point where they are defined.

**Version 2.7 (2009-07-08)**: 

- New call graph for functions showing a function in context:
with all the paths from its callers and all called functions.

-  When saving refactored code CScout will verify that the
renamed identifiers do not clash with other existing identifiers.
This requires a complete re-processing of the code.
A corresponding option can disable this feature.

-  Added function argument refactorings page through which
function argument refactorings can be reviewed, changed, and
deactivated.

-  Optimize file handling during the post-processing phase.
In a typical use case this results in a 37% reduction in
processing time.

- Correct handling of function names appearing as function pointers,
when refactoring function arguments.

- Corrected dot syntax error when generating  graphs with no global options set.

- Better error handling when refactored files cannot be renamed or unlinked.

- Preprocessor expressions are evaluated as if the have the same
representation intmax_t/uintmax_t, rather than long.

- Fix a crash that occurred when processing Apache httpd 1.3.27.

- Fix to avoid *csmake* getting confused by *ccache*.

**Version 2.6 (2008-12-05)**: 

- Add support for the display of graph files in PDF and PNG format.

- Add options for passing graph, node, and edge properties to *dot*.

-  *cswc* now deals correctly with scoped definitions for files.

- Correct the generation of obfuscated identifiers in platforms
where they were invalid.

- Support the unification of *yacc* terminal symbols with the
corresponding `y.tab.h` macros.

- Correct extraneous line spacing in source code listings of files
containing carriage returns.

- Fix the display of graphs with empty nodes.

- Correct the handling of identifiers declaring a function at
an inner scope with no storage-class specifier (ISO C 6.2.2-5.)

**Version 2.5 (2008/11/17)**: 

- Each file's web page now provides links for generating
compile-time, control, and data dependency graphs.

- Identifier queries can now provide the functions where an identifier
appears.

- Bring in line the semantics of the `include_next`
preprocessor directive with those of *gcc*.

- *Yacc* identifiers now live in a separate namespace.
Terminal symbols are still visible in C code as ordinary identifiers,
but the potential for clashes between non-terminal names and C identifiers
has been eliminated.
This change also fixes the handling of rules for *yacc* untyped non-terminals,
and the calculation of file-local identifier metric in *yacc* files.

-  The provided definition files match closer the *gcc* builtins.

-  The file details page now provides links for listing functions defined
in the file and their call graph.

-  The main page and the individual file details pages now provide options
for generating include graphs. 

- 
In file, function, and identifier queries one can now specify to
exclude results from filenames matching a particular regular expression.

-  Take into account the appropriate order of CScout configuration
directories when saving options.

-  CScout's web interface received a facelift, and many pages should now
be clearer and easier to navigate.

- The second tab appearing in a source code's line is now correctly expanded.

- An option allows the addition of URLs in *dot* output.

- Remove redundancy from the saved options file.

**Version 2.4 (2008/07/15)**: 

- The web front-end now includes support for a multiuser read-only
browsing mode, and a log file.

- The web front end now includes support for directory browsing.

- 
The `include_next` *gcc*-specific directive now
works correctly, even when preceded in its file by other include
directives.

- 
Allow macro directives inside a macro-function call.
According to the C standard the behavior of this is undefined,
but *gcc* preprocessor does the right thing.

-  The web front-end now provides functionality for refactoring
the arguments of function calls.
A template mechanism allows changing the order of function arguments,
removing arguments, and introducing new ones.

-  The web front-end now provides functionality for hand-editing files.

- Under Windows the specified read-only prefix is case and path separator
insensitive.

-  Elements defined through the CScout definition file are now considered
read-only.

-  Workspace compiler: Correctly handle scoping in nested units.

-  Workspace compiler: New readonly command for units other than files.

-  A new option controls the depth of call graphs.

-  Added functionality to interactively explore the tree of
a function's calling and called functions.

-  The MIME type of SVG call graphs is now compatible Firefox's builtin
SVG viewer.

**Version 2.3 (2008/04/07)**: 

-  Each defined function is now associated with a comprehensive
set of metrics. These appear in the function's page, can be queried
against all functions, and are also summarized.
This is a list of maintained function metrics.

  - Number of characters

  - Number of comment characters

  - Number of space characters

  - Number of line comments

  - Number of block comments

  - Number of lines

  - Maximum number of characters in a line

  - Number of character strings

  - Number of unprocessed lines

  - Number of C preprocessor directives

  - Number of processed C preprocessor conditionals (ifdef, if, elif)

  - Number of defined C preprocessor function-like macros

  - Number of defined C preprocessor object-like macros

  - Number of preprocessed tokens

  - Number of compiled tokens

  - Number of statements or declarations

  - Number of operators

  - Number of unique operators

  - Number of numeric constants

  - Number of character literals

  - Number of if statements

  - Number of else clauses

  - Number of switch statements

  - Number of case labels

  - Number of default labels

  - Number of break statements

  - Number of for statements

  - Number of while statements

  - Number of do statements

  - Number of continue statements

  - Number of goto statements

  - Number of return statements

  - Number of project-scope identifiers

  - Number of file-scope (static) identifiers

  - Number of macro identifiers

  - Total number of object and object-like identifiers

  - Number of unique project-scope identifiers

  - Number of unique file-scope (static) identifiers

  - Number of unique macro identifiers

  - Number of unique object and object-like identifiers

  - Number of global namespace occupants at function's top

  - Number of parameters

  - Maximum level of statement nesting

  - Number of goto labels

  - Fan-in (number of calling functions)

  - Fan-out (number of called functions)

  - Cyclomatic complexity (control statements)

  - Extended cyclomatic complexity (includes branching operators)

  - Maximum cyclomatic complexity (includes branching operators and all switch branches)

  - Structure complexity (Henry and Kafura)

  - Halstead volume

  - Information flow metric (Henry and Selig)

-  A number of file-related metrics have been added:

  -  Number of defined project-scope functions 

  -  Number of defined file-scope (static) functions 

  -  Number of defined project-scope variables 

  -  Number of defined file-scope (static) variables 

  -  Number of declared aggregate (struct/union) members 

  -  Number of declared aggregate (struct/union) members 

  -  Number of complete enumeration declarations 

  -  Number of declared enumeration elements 

  -  Number of processed C preprocessor conditionals (ifdef, if, elif) 

  -  Number of defined C preprocessor function-like macros 

  -  Number of defined C preprocessor object-like macros 

- A new identifier query in the web interface lists function identifiers that should be made static.

- Newlines in string literals are now reported as errors.

- 
Trigraph support now disabled by default.  It can be explicitly enabled
through the new -3 switch.

-  The contents of the columns BASEFILEID and DEFINERID of the SQL table
INCTRIGGERS were reversed.
Their contents now correspond to their definitions.

- 
Correct support for initializer designators in conjunction with serial
initialization of aggregate elements.

-  Correctly unify identifiers in function declarations that are
declared as static, and are subsequently defined without a storage
class specifier.

- Fixed a relatively rare error in the ordinal numbering of function name
elements in the FUNCTIONID table.

-  The `__alignof__` *gcc* extension now also supports expressions,
in addition to types.

-  On 64-bit architectures fields containing pointers are stored in a
BIGINT database field.

-  MySQL dumps now start with SET SESSION sql_mode=NO_BACKSLASH_ESCAPES.
This prevents problems with the interpretation of backslashes stored in
strings and comments.

-  Fixed syntax error when a `typedef` followed a structure
initializer.

-  Put number of unprocessed lines in the database and print it as a warning. 

-  Added support for Microsoft's __try __except __finally __leave extensions. 

- Correctly parse a labeled statement appearing in the then
clause of an if statement.
This change tightens-up the use of the (deprecated) *gcc* extension
of allowing a label at the end of compound statement.
Following the change, only a single label is allowed.

-  Type definitions in an `else` block are now correctly
handled.

-  An enumeration list can be empty (Microsoft extension). 

-  Correct processing of assembly line comments (starting with a ;)
inside Microsoft inline assembly blocks. 

-  Fix a potential crash when processing a project
under the `-r` option. 

**Version 2.2 (2006/09/29)**: 

-  An new accompanying command, *csmake* can automatically
generate *CScout* processing scripts by monitoring a project's
build process.

-  Identical files in different locations are identified and presented in the file information page and through the file metrics. 

-  Facility to display the call path from one function to another. 

-  Identifiers occuring at the same place in identical files are considered to be the same.

-  All query results are presented through a page selection interface. 

-  New option to display call graphs as GIF images avoiding the need for an SVG plugin for medium-sized graphs. 

-  The SQL backend supports four additional tables: STRINGS, COMMENTS, FILEPOS, FILECOPIES.

- Allow attribute declarations to follow labels (*gcc* extension).

- Support indirect `goto` labels (*gcc* extension).

- Support (ANSI-style) nested function definitions (*gcc* extension).

- The macro expansion algorithm follows more closely
the C standard specification.

- Allow braces around scalar initializers (common extension).

- Macro calls in function arguments now get recorded as calls from the
enclosing function, rather than the function being called.

-  Significantly faster file post-processing for the web and the SQL interface in large
projects.

-  Array designators can be denoted through a range (*gcc* extension). 

-  Support for symbolic operands in *gcc* asm constructs.

-  Allow `__typeof__` declarations to be preceded by type qualifiers. 

-  Correctly handle `__typeof__` of objects with a storage class within typedef declarations. 

-  The order of include file searching now matches more closely
that of other compilers: absolute file names are never searched in
the include file path, and non-system files are first searched relative
to the directory of the including file. 

-  Allow empty initializers and compound literals.  (*gcc* extension)

-  Support for the `alignof` operator (*gcc* extension)

-  The equals sign following an initializer designator is optional
(*gcc* extension).  

-  A declaration expression can be used as the first expression of a for
statement. (C99) 

-  `__typeof` can also have as its argument a type name 

-  Support for designators in compound literals. (C99) 

-  Correctly handle preprocessing tokens with values close to UINT_MAX.

-  Correctly evaluate preprocessor expressions involving a mixture of
signed and unsigned values.

-  Correct handling of logical OR and logical AND preprocessor expressions. 

-  In query results of files sorted by a metric, groups of files with the same
 value had only one member of the group displayed.  This has now been fixed.

-  Correct handling of implicit function declarations. 

-  Correct handling of character constants containg a
double-quote character when collecting metrics, database
dumping, and obfuscating. 

-  Function declarations appearing in multiple identical files now appear as a
single function.

- 
Correct operation when the same project is encountered in the input
more than once.

-  Correctly support the `index[array]` construct.

-  Correct typing of arithmetic involving arrays.

-  Will not register function typedefs as function declarations.

-  Don't issue an error message when a # appears in a skipped #error message

-  Correct handling of C99 nested initialization designators. 

-  More consistent support for *gcc* keyword synonyms,
such as `__inline` and `__restrict__`.

-  Correct expansion of a macro following a token named after a
function-like macro.

-  The supplied *gcc* definition files contain support for
the `__builtin_expect` function.  

-  Correct operation of `typedef`s involving a
`__typeof` construct.

-  Will not report unneeded included files for (the rare case of)
compilation unit source files that are also included in other compilation units.
The unused included file report for such cases could incorrectly identify
files that were required for compilation as unneeded.

- Support for the C99 `_Bool` data type.

**Version 2.1 (2005/05/14)**: 

- 
The mixing of NULL with object pointers in conditional expressions
is now closer to the ANSI C rules.

- Support for the C99 `restrict` and `inline` keywords.

- An identifier declared with an extern storage-class specifier in
a scope in which a static declaration of that identifier is visible
will inherit that static declaration without an error.

- Allow struct/union declarations with only an empty declaration list (*gcc* extension).

- Correct preprocessing of floating-point numbers starting with a decimal point.

- Correctly handle the remainder operator in C preprocessor expressions.

- Allow absolute filename specifications in all #include directives.

- Report writable files that a given file must include.

- Facility to review and selectively deactivate identifier substitutions.

- A new option allows a regular expression to control the location where
modified files will be written.

-  File, identifier, and function queries, display the number of elements
matched.

-  Correct handling of the read-only pragma under Win32 platforms.

-  Will not report (erroneous) file metrics for empty file sets.

- Accept *gcc* synonyms __typeof__, __label, and __attribute.

-  Correctly handle structure initializations through typedefs that were
initially declared with an incomplete structure tag.

- Correctly calculate metrics in files with *-decorated block comments.

- Correctly deduce writable Unix files.

- Correctly identify as the same a function whose name is composed
through token concatenation, and occurs in two projects.

**Version 2.0 (2004/07/31)**: 

- Monitor calls across functions and macros,
generating call graphs in various formats,
including hyperlinked SVGs.
A new category provides canned and customized queries on functions
and macros.

- Monitor parts of files not processed due to conditional
compilation.
The results are available as a new file metric category.
In addition, unprocessed parts can be identified in
source listings in a different color.

-  Global options can be saved to a file, and loaded from it
on startup.

-  File queries can now specify a sorting order for the file results.

-  A new file query lists files with unprocessed lines, ordered
by the number of unprocessed lines.

-  The presentation of file lists has been improved

-  Will not report unused included files that are included with the
same directive that also includes used files.
This can happen when the same file is compiled multiple times with different
include paths or when a file is included by expanding a macro.
Problem spotted by Alexios Zavras in the FreeBSD kernel report.

-  Allow `typedef`ed pointers and arrays to be further
qualified with e.g. `const` or `volatile`.
Problem reported by Walter Briscoe.

-  The second argument of a conditional expression can be
omitted (*gcc* extension).

-  Fix assertion generated when processing a *yacc* file without having
defined a yyparse function.

**Version 1.16 (2003/08/27)**: 

-  Declarations can be intermixed with statements (C99).

-  `__typeof` can have as its argument an expression
and not only an identifier.

-  Support for C99 variable number of arguments preprocessor macros.

-  Allow `case` expression ranges (*gcc* extension).

-  Recognise `__atribute__(__unused__)` for determining which
identifiers should not be reported as unused (*gcc* extension).

-  Command-line option to generate a wrongly scoped identifier and unused 
include file and identifier warning report.

-  Separate identifier attribute for enumeration constants.
This allows us stop incorrectly categorizing them as having global
(compilation unit) visibility.

-  Error reporting format is now compatible with *gcc*.

-  Dereferencing a function yields a function (common extension).

-  Command-line option to process the file and exit.

-  Document processing of the FreeBSD kernel.

-  Correct typing of assembly-annotated declarators.

-  Fixed assertion failure that could be caused when parts of concatenated
identifiers were no longer available (e.g. when processing files
with the `-m T` option.)

-  Correct handling of macro parameters that match other macros and
are followed by a concatenation operator (they were erroneously replaced).

-  Add workaround for *gcc* `__builtin_va_copy` in the provided
definition files.

-  Corrected the handling of `main()` in the example definition
files.

**Version 1.15 (2003/08/06)**: 

-  Plugged another memory leak.
All remaining memory leaks are caused by STL caching and should be of a
constant overhead.

**Version 1.14 (2003/08/03)**: 

-  Support locally declared labels (`__label__`) (*gcc* extension).

-  Allow statement labels without a following statement (*gcc* extension).

-  Allow assignment to case expressions (common extension)

-  Support C99 initialization designators.

-  Support aggregate member initialization using the member: value syntax (*gcc* extension)

-  Major memory leak plugged (a missing virtual destructor).
*Cscout* will now consume about 7 times less memory.

-  In `#if` and `#elif` directives expand macros
before processing the `defined` operator.

-  Support the vararg preprocessor macro syntax (*gcc* extension).

-  Allow empty member declarations in aggregates (*gcc* extension).

-  Allow the declaration of empty structures or unions (*gcc* extension).

-  An `__asm__` declaration can be used instead of a function's body (*gcc*).

-  Correct typing of the conditional operator's return type
when one argument is a pointer and the other NULL.

-  New *-m* option to specify identifiers to track.
Enormous memory savings at the expense of unsound operation.

-  Display an error when a file does not end with a newline

-  Allow *yacc* %union declaration to end with a ;

-  Accept the #ident preprocessor directive (*gcc* extension)

-  Fixed preprocessor bug:
multiple expansions of the same function-like macro inside another macro would
fail.

-  Correctly handle concatenation of empty macro arguments.

-  Correctly handle function prototypes inside old-style argument declarations.

-   Do not replace strings or characters matching the name of a macro formal argument.

-  Accept an empty translation unit (common extension).

-  Adding a pointer to an integer now correctly yields a pointer
(adding an integer to a pointer already worked correctly)

-  Support C99 compound literals.

-  Correct typing of compound statements as expressions (*gcc* extension)

-  Improved configuration definition files for the GNU C compiler

**Version 1.13 (2003/07/07)**: 

-  Handle GNU `__asm__` extension with a single operand

-  -d options to *cswc* for specifying configuration directory

-  `-p` command-line option to specify the web server port

-  Small corrections in the FreeBSD definition files.

-  Intel IA-64 and AMD-64 support

**Version 1.12 (2003/06/23)**: 

-  New query: writable identifiers that should be made static

-  URLs now work on 64-bit architectures

-  Improved distribution format

-  Sun SPARC-64 support

**Version 1.10 (2003/06/22)**: 

-  Allow GNU __asm__ statements with a single operand.

-  GNU __asm__ statements can also appear at file scope.

-  Accept older "name = {action statements}" *yacc* syntax.

-  Can handle untyped *yacc* specifications.

-  Correctly handle struct/union type specifier followed by a type qualifier.

-  Small corrections in the supplied definition files.

**Version 1.9 (2003/06/19)**: 

-  Now distributed with ready-to-run example;
     the *awk* source code.
     No setup required, just unpack the distribution and run.

-  Can now also process *yacc* files

-  Fixed syntax error in workspace compiler source code

-  Correctly documented -P switch as -E

-  Added stdlib.h generic header

**Version 1.8 (2003/06/16)**: First public release
