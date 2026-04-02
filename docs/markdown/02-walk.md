# Walkthrough

If you are impatient, you can get an immediate feeling of *CScout*,
by building it (`make`),
entering the `example` directory and typing

- `../src/build/cscout awk.cs`

You will then be able to use *CScout* and your browser
to explore the source code of
the [one true awk](http://cm.bell-labs.com/who/bwk/index.md).

For a more structured walkthrough, read on.
Consider the following C file, `idtest.c`

```c
#define getval(x) ((x).val)

struct number {
	int id;
	double val;
} n;

struct character {
	int id;
	char val;
} c;

static int val;

main(int argc, char *argv[])
{
	int val;

	if (argc > 2)
		goto val;
	return getval(n) + getval(c);
	val: return 0;
}
```

Even though the file forms a contrived example, it will serve us to illustrate
the basic concepts behind *CScout*'s operation.
Consider what would the correct renaming of one of the identifiers
named `val` entail.
*CScout* will help us to automate this process.

Although, we are dealing with a single file we need to specify its processing
within the context of a workspace.  
In a realistic concept a workspace will specify how numerous projects
consisting of multiple files will be processed; think of a workspace
as a collection of Makefiles.
*CScout* will operate across the many source files and related
executables in the same way as it operates on our example
file `idtest.c`.

A *workspace* specifies the set of files on which *CScout*
will operate.
Each workspace consists of a number of *projects*;
a project is a set rules for linking together C
files to form an executable.
The workspace definition file is in our case very simple:

```
workspace example {
	project idtest {
		file idtest.c
	}
}
```

Our workspace, named `example`, consists of a single
project, named `idtest`, that consists of a single
C source file, `idtest.c`.

Our first step will be to transform the declarative workspace definition file
into a processing script:
a file with imperative processing directives that *CScout* will
handle.

```
prompt> cswc example.csw >example.c
```

We then invoke *CScout* on the processing script
(the compiled workspace definition file) `example.c`.

```
prompt> cscout example.c
Processing workspace example
Processing project idtest
Processing file idtest.c
Done processing file idtest.c
Done processing project idtest
Done processing workspace example
Post-processing our_path/example.c
Post-processing our_path/idtest.c
Processing identifiers
100%
We are now ready to serve you at http://localhost:8081
```

The output of *CScout* is quite verbose;
when processing a large source code collection, the messages will
serve to assure us that progress is being made.

The primary interface of *CScout* is Web-based, so once our files
have been processed, we fire-up our Web browser and navigate to the
*CScout*'s URL.
We leave the *CScout* process running; its job from now on
will be to service the pages we request and perform the operations we
specify.

Our browser will show us a page like the following:
  

File metrics

- 
Browse file tree  
-  All files
-  Read-only files
-  Writable files
-  Files containing unused project-scoped writable identifiers
-  Files containing unused file-scoped writable identifiers

-  Writable .c files without any statements
-  Writable files containing unprocessed lines
-  Writable files containing strings
-  Writable .h files with #include directives
-  Specify new file query

### File Dependencies

-  File include graph: writable files, all files-  Compile-time dependency graph: writable files, all files-  Control dependency graph (through function calls): writable files, all files-  Data dependency graph (through global variables): writable files, all files

### Functions and Macros

-  Function metrics
-  All functions
-  Non-static function call graph-  Function and macro call graph-  Project-scoped writable functions
-  File-scoped writable functions

-  Writable functions that are not directly called
-  Writable functions that are called exactly once
-  Specify new function query

Identifier metrics

-  All identifiers
-  Read-only identifiers
-  Writable identifiers
-  File-spanning writable identifiers
-  Unused project-scoped writable identifiers
-  Unused file-scoped writable identifiers

-  Unused writable macros
-  Writable variable identifiers that should be made static
-  Writable function identifiers that should be made static
-  Specify new identifier query

### Operations

-  Global options

 — save global options
-  Identifier replacements
-  Select active project
-  Save changes and continue
-  Exit — saving changes

-  Exit — ignore changes

| Files | Identifiers |
| --- | --- |

Main page
 — Web: Home
Manual
  

---
CScout

| Files | Identifiers |
| --- | --- |
| Files | Identifiers | Identifiers |

In our first example we will only rename an identifier, 
but as is evident from the page's links *CScout* provides us 
with many powerfull tools.

By navigating through the links
*All files*,
*idtest.c*, and
*Source code with identifier hyperlinks* we can see the
source code with each recognised identifier marked as a hyperlink:
getval(x) ((x).val)
  

  

struct number {
  

        int id;
  

        double val;
  

} n;
  

  

struct character {
  

        int id;
  

        char val;
  

} c;
  

  

static int val;
  

  

main(int argc, char *argv[])
  

{
  

        int val;
  

  

        if (argc > 2)
  

                goto val;
  

        return getval(n) + getval(c);
  

        val: return 0;
  

}
  

---
`

Main page

| Source Code With Identifier Hyperlinks: your_path/idtest.c (Use the tab key to move to each marked element.)#define |
| --- |

Clicking on the first identifier `val` (in the macro definition)
we are taken to a page specifying the identifier's details.
There we can specify the identifier's new name, e.g. `value`.
your_path/idtest.c - marked source

### Dependent Files (All)

- your_path/idtest.c - marked source

Main page

| Identifier: val   Read-only: No Tag for struct/union/enum: No Member of struct/union: Yes Label: No Ordinary identifier: No Macro: No Undefined macro: No Macro argument: No File scope: No Project scope: No Typedef: No Crosses file boundary: No Unused: No  Matches 3 occurence(s)  Appears in project(s):   idtest   Substitute with:      Dependent Files (Writable) |
| --- |

Clicking on the *marked source* hyperlink,
*CScout* will show us again the corresponding source
code, but with only the identifiers `val` matches marked as
hyperlinks:

val)
  

  

struct number {
  

        int id;
  

        double val;
  

} n;
  

  

struct character {
  

        int id;
  

        char val;
  

} c;
  

  

static int val;
  

  

main(int argc, char *argv[])
  

{
  

        int val;
  

  

        if (argc > 2)
  

                goto val;
  

        return getval(n) + getval(c);
  

        val: return 0;
  

}
  

---
`

Main page

| Identifier val: C:\dds\src\Research\cscout\refactor\idtest.c (Use the tab key to move to each marked element.)#define getval(x) ((x). |
| --- |

The marked identifiers will be all the ones and the only ones the replacement
we specified will affect.
Similarly we can specify the replacement of the `val`
label, the static variable, or the local variable;
each one will only affect the relevant identifiers.

Selecting the hyperlink
*Exit - saving changes* from the 
*CScout*'s main page will commit our changes,
modifying the file `idtest.c`.
