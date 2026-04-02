# The cscut Command Manual Page


## Name

cscut - Extract portions of a CScout processing script

## Synopsis

**cscut**
[**-b**]
[**-e**]
[**-F** *file-list* | **-f** *file* | **-p** *project*]
*cscout-file*

## Description

The *cscut* command-line tool extracts specific portions of a
*CScout* processing script.
It provides a flexible way to select parts of the *CScout* processing
script based on files or projects.

The following options are available:

**-b**  
Output from the file's beginning until the specified point.

**-e**  
Output from the specified point to the end of the file.

**-F \fIfile-list**  
Output a script to process only the files specified in *file-list*,
where *file-list* is a file containing a list of filenames.

**-f \fIfile**  
Output a script to process only the specified *file*.

**-p \fIproject**  
Output a script to process only the specified *project*.

The last argument, *cscout-file*, specifies the *CScout*
processing script to be processed.

## Example

Below are some typical use cases for *cscut*.

Extract a script to process the file \fCx86.cs:

.DS
.ft C
.nf
cscut -f /home/dds/src/linux/arch/x86/kvm/x86.c make.cs >x86.cs
.ft P
.fi
.DE

Extract a script to process the files listed in *file-list.txt*:

.DS
.ft C
.nf
cscut -F file-list.txt make.cs >files.cs
.ft P
.fi
.DE

Extract a script to process only the project \fC.tmp_vmlinux3:

.DS
.ft C
.nf
cscut -p /home/dds/src/linux/.tmp_vmlinux3 >linux.cs
.ft P
.fi
.DE

Extract a script to process from the file \fCexport.c and onward:

.DS
.ft C
.nf
cscut -f /home/dds/src/linux/fs/ocfs2/export.c linux.cs >ocfs2-export.cs
.ft P
.fi
.DE


## See also

cscout(1),
cssplit(1),
csmake(1)

## Author

(C) Copyright 2024 Diomidis Spinellis.