# The cscout Command Manual Page


## Name

cscout - C code analyzer and refactoring browser

## Synopsis

**cscout**
[**-bCcrv3**]
[**-d D**]
[**-d H**]
[**-E** *file specification*]
[**-l** *log file*]
[**-q**]
[**-p** *port*]
[**-R** *specification*]
[**-M** *files*]
[**-m** *specification*]
[**-t** *sname*]
[**-o** | **-S** *db* | **-s** *db* | **-M** *files*]
*file*

## Description

*CScout* is a source code analyzer and refactoring browser for collections
of C programs.
It can process workspaces of multiple projects (we define
a project as a collection of C source files that are linked together)
mapping the complexity introduced by the C preprocessor back into the
original C source code files.
*CScout* takes advantage of modern hardware
advances (fast processors and large memory capacities) to analyze C
source code beyond the level of detail and accuracy provided by current
compilers and linkers.
The analysis *CScout* performs takes into account
the identifier scopes introduced by the C preprocessor and the C language
proper scopes and namespaces.

*CScout* as a source code analyzer can:
annotate source code with hyperlinks to each identifier
list files that would be affected by changing a specific identifier
determine whether a given identifier belongs to the application
or to an external library based on the accessibility and location of the
header files that declare or define it
locate unused identifiers taking into account inter-project
dependencies
perform queries for identifiers based on their namespace,
scope, reachability, and regular expressions of their name and the
filename(s) they are found in,
perform queries for files, based on their metrics, or properties
of the identifiers they contain
monitor and report superfluously included header files
provide accurate metrics on identifiers and files

More importantly,
*CScout* helps you in refactoring code by identifying dead objects
to remove, and
can automatically perform accurate global *rename identifier*
refactorings.
*CScout* will automatically rename identifiers
taking into account the namespace of each identifier: a renaming of
a structure tag, member, or a statement label will not affect variables
with the same name
respecting the scope of the renamed identifier: a rename can affect
multiple files, or variables within a single block, exactly matching
the semantics the C compiler would enforce
across multiple projects when the same identifier is defined in
common shared include files
occuring in macro bodies and *parts* of other identifiers,
when these are created through the C preprocessor's token concatenation
feature

This manual page describes the *CScout* invocation and command-line
options.
Details about its web interface, setup, and configuration can be
found in the online hypertext documentation and at the project's home page
http://www.spinellis.gr/cscout.


## Options

Create a *ctags*-compatible tags file.
Tens of editors and other tools can utilize tags to help you navigate
through the code.
In contrast to other tag generation tools, the file that *CScout*
creates also includes information about entities dynamically generated
through macros.
Exit immediately after processing the specified files.
Useful, when you simply want to check the source code for errors or when
you want to create a *tags* file.
Display the \fC#define directives being processed on the standard
output.
Display the (mainly header) files being included on the standard output.
Each line is prefixed by a number of dots indicating the depth
of the included file stack.
Preprocess the file specified with the regular expression given as the
option's argument and send the result to the standard output.
The web server will listen for requests on the TCP port number specified.
By default the *CScout* server will listen at port 8081.
The port number must be in the range 1024-32767.
Specify names of CSV input files to merge into
specified generated output files.
These merge sharded database tables involving equivalence classes.
Further details can be found in the file \fCeclasses.sql, which
utilizes this feature.
Specify the type of identifiers that *CScout* will monitor.
The identifier attribute specification is given using the syntax:
Y**|**L**|**E**|**T**[:***attr1**][:**\fIattr2***]**...
The meaning of the first letter is:
.RS 4
Match anY of the specified attributes
Match alL of the specified attributes
ExcludE the specified attributes matched
ExacT match of the specified attributes
.RE

Allowable attribute names and their corresponding meanings are:
.RS 4
Unused identifier
Writable identifier
Read-only identifier
Identifier stored in a macro
Defined as a function-like macro
Used to derive a preprocessor constant, i.e. used in a \fC#if or
\fC#include directive or in a \fCdefined() expression
Macro's value is used as a string (pasting or stringization)
in the preprocessor
Macro value seen as a C compile-time constant
Macro value seen as not a C compile-time constant
Macro value seen as a C compile-time constant
Macro value seen as not a C compile-time constant
Tag for a struct/union/enum
Member of a struct/union
Label
Ordinary identifier
(note that enumeration constants and typedefs belong to the ordinary
identifier namespace)
Preprocessor macro
Undefined preprocessor macro
Preprocessor macro argument
Identifier with file scope
Identifier with project scope
Typedef
Enumeration constant
Yacc identifier
C function
.RE

The *-m* flag can provide enormous savings on the memory *CScout*
uses (specify e.g. *-m Y:pscope* to only track project-global identifiers),
but the processing *CScout* performs under this flag is *unsound*.
The flag should therefore be used only if you are running short
of memory.
There are cases where the use of preprocessor macros
can change the attributes of a given identifier shared
between different files.
Since the *-m* optimization is performed after each single file is
processed, the locations where an identifier is found may be misrepresented.
Report on the standard error output warnings about unused and wrongly
scoped identifiers and unused included files.
The error message format is compatible with *gcc* and can
therefore be automatically processed by editors that recognize this
format.
Suppress the progress messages that
*CScout* normally prints on its standard error output
while processing and analyzing files.
This is useful when *CScout* is invoked programmatically,
for example from an editor or IDE plugin,
where such messages would appear as unnecessary noise.
Note that fatal error messages and the server readiness notification
are always printed regardless of this option.
Display  the *CScout* version and copyright information and exit.
Implement support for trigraph characters.
Operate in multiuser browse-only mode.
In this mode the web server can concurrently process multiple requests.
All web operations that can affect the server's functioning
(such as setting the various options, renaming identifiers,
refactoring function arguments, selecting a project, editing a file,
or terminating the server) are prohibited.
Call graphs are truncated to 1000 elements (nodes or edges).
Output the database schema as an SQL script.
Specify *help* as the database dialect to obtain a list of
supported database back-ends.
Dump the workspace contents as an SQL script.
Specify *help* as the database dialect to obtain a list of
supported database back-ends.
Merge the specified
*eclasses*, *ids*, and *functionids* files that contain
token, id, and functionid, details into new records
saved in three further corresponding files.
These can be directly imported into the *tokens*,
*ids*, and *functionids* tables.
Specify the location of a file where web requests will be logged.
Generate call graphs and exit.
The option can be specified multiple times.
The specification is the type of desired graph
(*fgraph.txt* for a file dependency graph or
*cgraph.txt* for a function and macro call graph),
optionally followed by parameters appearing in the corresponding
URL of the *CScout* web interface.
The generated text file contains one space-delimited relationship per line.
It can be further processed by tools such sas *awk* and *dot*
to produce graphical output as shown in the following examples.
.DS
.nf
.ft C
# Create a function call graph and a compile-time file dependency graph
cscout -R cgraph.txt -R fgraph.txt?gtype=C.
# Convert the generated call graph into an SVG diagram
awk '
 BEGIN { print "digraph G {" }
 {print $1 "->" $2}
 END { print "}" }' cgraph.txt |
dot -Tsvg >cgraph.svg
.ft P
.fi
.DE
Generate SQL output for the named table.
By default SQL output for all tables is generated.
If this option is provided, then output only for the specified tables
will be generated.
It is the user's responsibility to list the tables required to avoid
breaking integrity constraints.
The option can be specified multiple times.
Create obfuscated versions of all the writable files of the workspace.


## Example

Assume you want to analyze three programs in \fC/usr/src/bin.
You first create the following project definition file,
\fCbin.prj.

.DS
.ft C
.nf
# Some small tools from the src/bin directory
workspace bin {
        ro_prefix "/usr/include"
        cd "/usr/src/bin"
        project cp {
                cd "cp"
                file cp.c utils.c
        }
        project echo {
                cd "echo"
                file echo.c
        }
        project date {
                cd "date"
                file date.c
        }
}
.ft P
.fi
.DE

Then you compile the workspace file \fCbin.prj
by running the *CScout* workspace compiler *cswc* on it,
and finally you run *cscout* on the compiled workspace file.
At that point you are ready to analyze your code and rename its identifiers
through your web browser.

.DS
.ft C
.nf
$ cswc bin.prj >bin.cs
$ cscout bin.cs
Processing workspace bin
Entering directory /usr/src/bin
Processing project cp
Entering directory cp
Processing file cp.c
Done processing file cp.c
Processing file utils.c
Done processing file utils.c
Exiting directory cp
Done processing project cp
Processing project echo
Entering directory echo
Processing file echo.c
Done processing file echo.c
Exiting directory echo
Done processing project echo
Processing project date
Entering directory date
Processing file date.c
Done processing file date.c
Exiting directory date
Done processing project date
Exiting directory /usr/src/bin
Done processing workspace bin
Post-processing /usr/home/dds/src/cscout/bin.c
[...]
Post-processing /vol/src/bin/cp/cp.c
Post-processing /vol/src/bin/cp/extern.h
Post-processing /vol/src/bin/cp/utils.c
Post-processing /vol/src/bin/date/date.c
Post-processing /vol/src/bin/date/extern.h
Post-processing /vol/src/bin/date/vary.h
Post-processing /vol/src/bin/echo/echo.c
Processing identifiers
100%
We are now ready to serve you at http://localhost:8081
.ft P
.fi
.DE


## See Also

*cscc*(1),
*cscut*(1),
*csmake*(1),
*csreconst*(1),
*cssplit*(1),
*csmerge*(1),
*cswc*(1).

## Author

(c) Copyright 2003-2026 Diomidis Spinellis.