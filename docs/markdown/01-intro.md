# Introduction

*CScout* is a source code analyzer and refactoring browser for
collections of C programs.
It can process workspaces of multiple projects (we define a project
as a collection of C source files that are linked together)
mapping the complexity introduced
by the C preprocessor back into the original C source code files.
*CScout* takes advantage of modern hardware advances (fast processors
and large memory capacities) to analyze C source code beyond the level
of detail and accuracy provided by  current compilers, linkers, and
other source code analyzers.
The analysis CScout performs takes into account the identifier scopes
introduced by the C preprocessor and the C language proper scopes and
namespaces.

*CScout* has already been applied on

-  Projects of tens of thousands of lines,
like the apache web server.
-  Projects above a million lines of code,
like the Linux (4.3MLOC), the FreeBSD (4.7MLOC) and the Windows Research
(0.9MLOC) kernels
-  The complete
[one true awk](http://cm.bell-labs.com/who/bwk/index.md)
source code (6600 lines - including a yacc grammar),
without modifying a single byte of the source code.

CScout as a source code analyzer can:

-  annotate source code with hyperlinks to each identifier
-  list files that would be affected by changing a specific identifier
-  determine whether a given identifier belongs to the application
or to an external library based on the accessibility and location of the
header files that declare or define it
-  locate unused identifiers taking into account inter-project 
dependencies
-  create static call graphs that include the use of function-like macros
-  perform queries for identifiers based on their namespace,
scope, reachability, and regular expressions of their name and the
filename(s) they are found in,
-  perform queries for files, based on their metrics, or properties
of the identifiers they contain
-  perform queries for functions and function like-macros,
based on their metrics, their type and scope, the names of callers and callees,
and the files they are declared in
-  monitor and report superfluously included header files
-  visually identify parts of files that were skipped during
preprocessing
-  provide accurate metrics on functions, identifiers, and files

More importantly,
*CScout* helps you in refactoring code by
identifying dead objects to remove, and
automatically performing accurate global *rename identifier*
refactorings,
and various function argument refactorings.
*CScout* will automatically rename identifiers

-  taking into account the namespace of each identifier: a renaming of
a structure tag, member, or a statement label will not affect variables
with the same name
-  respecting the scope of the renamed identifier: a rename can affect
multiple files, or variables within a single block, exactly matching
the semantics the C compiler would enforce
-  across multiple projects when the same identifier is defined in
common shared include files
-  occuring in macro bodies and *parts* of other identifiers,
when these are created through the C preprocessor's token concatenation
feature

Furthermore, *CScout* allows you to refactor the arguments
of functions and macros, introducing new arguments, deleting existing ones,
or changing their order.
