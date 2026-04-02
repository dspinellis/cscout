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
  

[File metrics](simul.html)

- 
[Browse file tree](simul.html)  
-  [All files](simul.html)
-  [Read-only files](simul.html)
-  [Writable files](simul.html)
-  [Files containing unused project-scoped writable identifiers](simul.html)
-  [Files containing unused file-scoped writable identifiers](simul.html)

-  [Writable .c files without any statements](simul.html)
-  [Writable files containing unprocessed lines](simul.html)
-  [Writable files containing strings](simul.html)
-  [Writable .h files with #include directives](simul.html)
-  [Specify new file query](simul.html)

### File Dependencies

-  File include graph: [writable files](simul.html), [all files](simul.html)-  Compile-time dependency graph: [writable files](simul.html), [all files](simul.html)-  Control dependency graph (through function calls): [writable files](simul.html), [all files](simul.html)-  Data dependency graph (through global variables): [writable files](simul.html), [all files](simul.html)

### Functions and Macros

-  [Function metrics](simul.html)
-  [All functions](simul.html)
-  [Non-static function call graph](simul.html)-  [Function and macro call graph](simul.html)-  [Project-scoped writable functions](simul.html)
-  [File-scoped writable functions](simul.html)

-  [Writable functions that are not directly called](simul.html)
-  [Writable functions that are called exactly once](simul.html)
-  [Specify new function query](simul.html)

[Identifier metrics](simul.html)

-  [All identifiers](simul.html)
-  [Read-only identifiers](simul.html)
-  [Writable identifiers](simul.html)
-  [File-spanning writable identifiers](simul.html)
-  [Unused project-scoped writable identifiers](simul.html)
-  [Unused file-scoped writable identifiers](simul.html)

-  [Unused writable macros](simul.html)
-  [Writable variable identifiers that should be made static](simul.html)
-  [Writable function identifiers that should be made static](simul.html)
-  [Specify new identifier query](simul.html)

### Operations

-  [Global options](simul.html)

 — [save global options](simul.html)
-  [Identifier replacements](simul.html)
-  [Select active project](simul.html)
-  [Save changes and continue](simul.html)
-  [Exit — saving changes](simul.html)

-  [Exit — ignore changes](simul.html)

| Files | Identifiers |
| --- | --- |

[Main page](simul.html)
 — Web: [Home](simul.html)
[Manual](simul.html)
  

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
[getval](simul.html)([x](simul.html)) (([x](simul.html)).[val](simul.html))
  

  

struct [number](simul.html) {
  

        int [id](simul.html);
  

        double [val](simul.html);
  

} [n](simul.html);
  

  

struct [character](simul.html) {
  

        int [id](simul.html);
  

        char [val](simul.html);
  

} [c](simul.html);
  

  

static int [val](simul.html);
  

  

[main](simul.html)(int [argc](simul.html), char *[argv](simul.html)[])
  

{
  

        int [val](simul.html);
  

  

        if ([argc](simul.html) > 2)
  

                goto [val](simul.html);
  

        return [getval](simul.html)([n](simul.html)) + [getval](simul.html)([c](simul.html));
  

        [val](simul.html): return 0;
  

}
  

---
`

[Main page](simul.html)

```
Source Code With Identifier Hyperlinks: your_path/idtest.c (Use the tab key to move to each marked element.)#define
```

Clicking on the first identifier `val` (in the macro definition)
we are taken to a page specifying the identifier's details.
There we can specify the identifier's new name, e.g. `value`.
[your_path/idtest.c](simul.html) - [marked source](simul.html)

### Dependent Files (All)

- [your_path/idtest.c](simul.html) - [marked source](simul.html)

[Main page](simul.html)

```
Identifier: val   Read-only: No Tag for struct/union/enum: No Member of struct/union: Yes Label: No Ordinary identifier: No Macro: No Undefined macro: No Macro argument: No File scope: No Project scope: No Typedef: No Crosses file boundary: No Unused: No  Matches 3 occurence(s)  Appears in project(s):   idtest   Substitute with:      Dependent Files (Writable)
```

Clicking on the *marked source* hyperlink,
*CScout* will show us again the corresponding source
code, but with only the identifiers `val` matches marked as
hyperlinks:

[val](simul.html))
  

  

struct number {
  

        int id;
  

        double [val](simul.html);
  

} n;
  

  

struct character {
  

        int id;
  

        char [val](simul.html);
  

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

[Main page](simul.html)

```
Identifier val: C:\dds\src\Research\cscout\refactor\idtest.c (Use the tab key to move to each marked element.)#define getval(x) ((x).
```

The marked identifiers will be all the ones and the only ones the replacement
we specified will affect.
Similarly we can specify the replacement of the `val`
label, the static variable, or the local variable;
each one will only affect the relevant identifiers.

Selecting the hyperlink
*Exit - saving changes* from the 
*CScout*'s main page will commit our changes,
modifying the file `idtest.c`.
