# Defining Workspaces

A workspace definition provides *CScout* with instructions for parsing 
a set of C files;
the task that is typically accomplished when compiling programs 
through the use of makefiles.
*CScout* must always process all its source files in a single batch,
so running it for each file from a makefile is not possible.
Workspace definition files provide facilities for 
specifying linkage units (typically executable files - 
*projects* in the workspace definition file parlance)
grouping together similar files and specifying include paths,
read-only paths, and macros.

Workspace definition files are line-oriented and organized around C-like blocks.
Comments are introduced using the # character.
Consider the following simple example:

```
workspace echo {
	project echo {
		cd "/usr/src/bin/echo"
		file echo.c
	}
}
```

The above workspace definition consists of a single program (echo),
which in turn consists of a single source file (echo.c).

See how we could expand this for two more programs, all residing in 
our system's `/usr/src/bin` directory:

```
workspace bin {
	cd "/usr/src/bin"
	ro_prefix "/usr/include"
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
```

In the new `bin` workspace we have factored out the common
source directory at the workspace level
(`cd "/usr/src/bin"`), so that each project only
specifies its directory relatively to the workspace directory
(e.g. `cd "date"`).
In addition, we have specified that files residing in the
directory `/usr/include` are to be considered read-only
(`ro_prefix "/usr/include"`).
This is typically needed when the user running *CScout*
has permission to modify the system's include files.
Specifying one or more read-only prefixes allows *CScout* to
distinguish between application identifiers and files, which you can
modify, and system identifiers and files, which should not be changed.

The *CScout* workspace compiler *cswc* will read from its standard input,
or from the file(s) specified on its command line,
a workspace definition and produce on its standard
output a processing script:
a C-like file that *CScout* can process.
You will have to redirect the *cswc* output to a file that
will then get passed as an argument to *CScout*.
