# The cscc Command Manual Page


## Name

cscc - CScout C compiler front end

## Synopsis

**cscc**
[*gcc(1) options*]

## Description

*cscc* is a wrapper around *gcc*.
commands.
When invoked instead of *gcc*,
it will compile the specified file(s) in the same way as *gcc*
would do.
As a side-effect it will monitor the invocation of
*gcc(1)*
and record its environment and arguments.
With those data it will generate a *CScout* project specification
that can be used to process the file(s) that were compiled.
The project specification is saved into a file named *make.cs*.

## Example

The following commands are often the only ones required to process
a few C files.

.DS
.ft C
.nf
cscc example.c
cscout make.cs
.ft P
.fi
.DE

If you want to use *cscc* with another C compiler, prepend
**CC=*compiler-name** to the \fIcscc* invocation,
as shown in the following example.

.DS
.ft C
.nf
CC=x86_64-w64-gcc cscc
.ft P
.fi
.DE


## See also

cscout(1),
csmake(1)

## Author

(C) Copyright 2006-2019 Diomidis Spinellis.