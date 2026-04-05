# Tailoring the Build Process to Generate the Processing Script

It is relatively easy to integrate *CScout* into an existing IDE
(such as *Eclipse*) or to provide an alternative method
for specifying workspaces by directly creating a processing script
from existing Makefiles.
A processing script (what results from compiling a workspace file)
is a C file containing a number of
`#pragma` preprocessor directives.
*CScout* uses the following pragmas:

**`#pragma echo "*STRING*"`: 
Will display the *STRING* on *CScout*'s standard
output when that part of the file is reached.

Example:

```c
#pragma echo "Processing workspace date\n"
```

**`#pragma ro_prefix "*STRING*"`: 
Will add *STRING* to the list of filename prefixes that mark
read-only files.
This is a global setting.

Example:

```c
#pragma ro_prefix "C:\gcc"
```

**`#pragma define_immutable *macro definition*`: 
Will define a macro similarly to a `#define` command, but
mark the definition as one that cannot be later be undefined or redefined.
This is useful for defining macros that handle compiler extensions
so that code cannot change them.

**`#pragma project "*STRING*"`: 
Will set the current project to *STRING*.
All identifiers and files processed from then on will belong to the given
project.

Example:

```c
#pragma project "date"
```

**`#pragma block_enter`: 
Will enter a nested scope block.
Two blocks are supported, the first
`block_enter` will enter the project scope
(linkage unit); the second encountered nested
`block_enter` will enter the file scope
(compilation unit).

**`#pragma block_exit`: 
Will exit a nested scope block.
The number  of
`block_enter` pragmas should match the number of
`block_exit` pragmas and there should never be more
than two `block_enter` pragmas in effect.

**`#pragma process "*STRING*"`: 
Will analyze (*CScout*'s equivalent to compiling) 
the C source file named *STRING*.

Example:

```c
#pragma process "date.d"
```

**`#pragma pushd "*STRING*"`: 
Will set the current directory to *STRING*, saving the
previous current directory in a stack.
From that point onward, all relative file accesses will start from
the given directory.

Example:

```c
#pragma pushd "cp"
```

**`#pragma popd `: 
Will restore the current directory to the one in effect
before a previously pushed directory.
The number  of
`pushd` pragmas should match the number of
`popd` pragmas.

**`#pragma set_dp "*STRING* *INTEGER*`: 
Enable the debug point for the file named *STRING*
at line *INTEGER*.
This can be used for targetted troubleshooting and debugging:
rather than enabling a debug point globally using the
`dbpoints` file, it is enabled at a particular
code instance using the `#pragma set_dp`.

Example:

```c
#pragma set_dp "macro.cpp" 486
```

**`#pragma includepath "*STRING*"`: 
Will add *STRING* to the list of directories used for searching
included files (the include path).

Example:

```c
#pragma includepath "/usr/lib/gcc-lib/i386-redhat-linux/2.96/include"
```

**`#pragma clear_include `: 
Will clear the include path, allowing the specification of a new one.

**`#pragma clear_defines `: 
Will clear all defined macros allowing the specification of new ones.
Should normally be executed before processing a new file.
Note that macros can be defined using the normal
`#define` C preprocessor directive.

The following is a complete example of a *CScout* processing
script.

```c
// workspace bin
#pragma echo "Processing workspace bin\n"
#pragma ro_prefix "/usr/include"
#pragma echo "Entering directory /usr/src/bin"
#pragma pushd "/usr/src/bin"
// project date
#pragma echo "Processing project date\n"
#pragma project "date"
#pragma block_enter
#pragma echo "Entering directory date"
#pragma pushd "date"
// file date.c
#pragma echo "Processing file date.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_include
#include "/home/dds/src/cscout/cscout_defs.h"
#include "/home/dds/src/cscout/cscout_incs.h"
#pragma process "date.c"
#pragma block_exit
#pragma echo "Done processing file date.c\n"
#pragma echo "Exiting directory date\n"
#pragma popd
#pragma block_exit
#pragma echo "Done processing project date\n"
#pragma echo "Exiting directory /usr/src/bin\n"
#pragma popd
#pragma echo "Done processing workspace bin\n"
```

## Case Study: Processing the FreeBSD Kernel (the Hard Way)

As a further example consider the steps for applying
*CScout* on the FreeBSD kernel,
before the existence of the *csmake* command.

1.  Configure a LINT or GENERIC version of each architecture's kernel.  

Example: `config LINT`
1.  Go to the compilation directory, update the dependencies
(`make depend`)
and compile
(`make`).
This step is used to create all automatically generated C and header files.
Also during this step note the include path used, in order to provide
*CScout* with the same specification.
1.  Remove the generated object files to force a *make* operation
to rebuild them (`rm *.o`).
1.  Replace the C compiler invocation command in the Makefile with an
appropriate series of shell commands.

```
.include "$S/conf/kern.pre.mk"
The code below was added after the line above
NORMAL_C= echo '\#pragma echo "Processing file ${.IMPSRC}\n"' >>kernel.cs ;\
      echo '\#pragma block_enter' >>kernel.cs ;\
      echo '\#pragma clear_defines' >>kernel.cs ;\
      echo '\#pragma clear_include' >>kernel.cs ;\
      echo '\#include "cscout_defs.h"' >>kernel.cs ;\
      for i in $(INCLUDES) ; \
      do \
              case $$i in \
              -nostdinc) continue ;; \
              -I-) continue ;; \
              esac ; \
              i=`echo $$i | sed 's/-I//'` ; \
              echo '\#pragma includepath "'$$i'"' >>kernel.cs ; \
      done ; \
      echo '\#define _KERNEL 1' >>kernel.cs ;\
      echo '\#pragma process "opt_global.h"' >>kernel.cs ;\
      echo '\#pragma process "${.IMPSRC}"' >>kernel.cs ;\
      echo '\#pragma block_exit' >>kernel.cs ;\
      echo '\#pragma echo "Done processing file ${.IMPSRC}\n"' >>kernel.cs
```

1.  Create a `cscout_incs.h` file for each different architecture.
1.  Remove kernel.cs
The existing file documents the way to do it.
1.  Run *make* on the custom Makefile
1.  Repeat for each different architecture
1.  Create a top-level *CScout* file to process all architectures:

```c
#pragma echo "Processing workspace FreeBSD kernel\n"

#pragma echo "Entering directory sys/i386/compile/LINT\n"
#pragma pushd "sys/i386/compile/LINT"
#pragma echo "Processing project i386\n"
#pragma project "i386"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/i386/compile/LINT\n"
#pragma popd
#pragma echo "Done processing project i386\n"
#pragma block_exit

#pragma echo "Entering directory sys/amd64/compile/GENERIC\n"
// [...]
// and so on for all architectures
// [...]
#pragma echo "Exiting directory sys/sparc64/compile/LINT\n"
#pragma popd
#pragma echo "Done processing project sparc64\n"
#pragma block_exit
```

Note that the
`block_enter` and
`block_exit` pragmas
are furnished by this top-level file.

The run of the above specification (2 million unique lines)
took 330 CPU minutes on a
Rioworks HDAMA (AMD64) machine 
(2x1.8GHz Opteron 244 (in UP mode) - AMD 8111/8131 chipset, 8192MB mem)
and required 1474MB of RAM.
These are the complete metrics:

| File metric | Total | Min | Max | Avg |
| --- | --- | --- | --- | --- |
| Number of characters | 62505770 | 0 | 1008345 | 14502 |
| Comment characters | 15921752 | 0 | 85059 | 3694 |
| Space characters | 7936401 | 0 | 73968 | 1841 |
| Number of line comments | 19 | 0 | 4 | 0 |
| Number of block comments | 176253 | 0 | 4337 | 40 |
| Number of lines | 2063096 | 0 | 27336 | 478 |
| Length of longest line | 337049 | 0 | 1867 | 78 |
| Number of C strings | 132519 | 0 | 19296 | 30 |
| Number of defined functions | 29584 | 0 | 333 | 6 |
| Number of preprocessor directives | 267542 | 0 | 27336 | 62 |
| Number of directly included files | 35408 | 0 | 1608 | 8 |
| Number of C statements | 679825 | 0 | 4465 | 157 |

| File metric | Total | Min | Max | Avg |
| --- | --- | --- | --- | --- |
| Number of characters | 62505770 | 0 | 1008345 | 14502 |
| Comment characters | 15921752 | 0 | 85059 | 3694 |
| Space characters | 7936401 | 0 | 73968 | 1841 |
| Number of line comments | 19 | 0 | 4 | 0 |
| Number of block comments | 176253 | 0 | 4337 | 40 |
| Number of lines | 2063096 | 0 | 27336 | 478 |
| Length of longest line | 337049 | 0 | 1867 | 78 |
| Number of C strings | 132519 | 0 | 19296 | 30 |
| Number of defined functions | 29584 | 0 | 333 | 6 |
| Number of preprocessor directives | 267542 | 0 | 27336 | 62 |
| Number of directly included files | 35408 | 0 | 1608 | 8 |
| Number of C statements | 679825 | 0 | 4465 | 157 |
| Number of C statements | 679825 | 0 | 4465 | 157 | 157 |
