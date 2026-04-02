# The csmake Command Manual Page


## Name

csmake - CScout make

## Synopsis

**csmake**
[
[**-d**]
[**-h**]
[**-k**]
[**-o** *output-file*]
[**-s** *cs-files-directory*]
[**-A**]
[**-T** *temporary-directory*]
[**-t** *time-file*]
[**-N** *rules-file*]
--
]
[*make(1) options*]

## Description

*csmake* is a wrapper around *make* and typical C compilation
commands.
When invoked instead of *make* on a clean (unbuilt) system,
it will build the system in the same way as *make*.
As a side-effect it will monitor the invocations of
*cc(1)*,
*gcc(1)*,
*clang(1)*,
*ar(1)*,
*ld(1)*, and
*mv(1)* commands and record their environment and arguments.
With those data it will generate a *CScout* project specification
that can be used to process the project that was compiled.
The project specification is saved into a file named *make.cs*.
Moreover, a separate CScout .cs file can be generated for each executable
in a specified
.I cs_files_directory.

To allow *csmake* to be used as a drop in replacement in
build sequences that execute multiple *make* commands,
you can create a \fC/tmp/csmake-spy, which will be used
to create rules for the superset of all *make*-generated executables.

*csmake* options can be passed through *CSMAKEFLAGS* environment variable.


## Options

Run in debug mode (it also keeps spy directory in place).
Print help message.
Keep temporary directory in place.
Specify the name of the generated specification file,
which is by default
.I make.cs .
A name of - will output the file on the standard output.
Specify the cross-compilation toolchain prefix, e.g. \fCaarch64-linux-gnu-.
This changes the names of monitored toolchain commands by prefixing
their name as specified, e.g. . \fCaarch64-linux-gnu-gcc and thus
allows *csmake* to be applied in a cross-compilation environment.
Create a separate CScout .cs file for each real executable.
Save the .cs files into *cs-files-directory* in the current directory.
Generate cs projects for static libraries.
Run on an existing rules file.
Set temporary directory.
Use *temporary-directory* as temporary directory,
remember to clear it if you use it multiple times.
Record C compiler timing information in the specified file.
The information recorded is the output of *time*(1) with
format \fC'%D %e %F %I %K %M %O %S %U.


## Examples

The following commands are often the only ones required to process
a typical C project.

.DS
.ft C
.nf
make clean
csmake
cscout make.cs
.ft P
.fi
.DE

If you want to use *csmake* with another C compiler, prepend
**CC=*compiler-name** to the \fIcsmake* invocation,
as shown in the following example.

.DS
.ft C
.nf
CC=x86_64-w64-gcc csmake
.ft P
.fi
.DE

The following commands create an inspectable \fCrules file,
and then use that to creat the final \fCmake.cs file.
(Note the obligatory \fC-- sequence used to terminate the *csmake*
arguments.)

.DS
.ft C
.nf
mkdir csmake-tmp
csmake -k -T csmake-tmp  --
csmake -N csmake-tmp/rules  --
.ft P
.fi
.DE


The following commands can be used to run
.I csmake
for a Debian package using the
.I dpkg-buildpackage
command.

.DS
.ft C
.nf
rm -rf tmp_dir && mkdir -p tmp_dir
CSMAKEFLAGS='-T tmp_dir -A -s cscout_projects -k --' \\
  MAKE=/usr/local/bin/csmake dpkg-buildpackage -b
.ft P
.fi
.DE


## See also

*cscout*(1),
*cscut*(1),
*cssplit*(1).

## Author

(C) Copyright 2006-2024 Diomidis Spinellis.