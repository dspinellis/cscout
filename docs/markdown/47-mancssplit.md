# The cssplit Command Manual Page


## Name

cssplit - Split a CScout processing script into smaller scripts

## Synopsis

.B cssplit
.RB [ -h ]
.B -s
.I N
.I file
.RI [ file\ ... ]

## Description

The *cssplit*(1) command-line program splits a *CScout*
processing script into *N* smaller scripts for parallel
or distributed processing.
This is useful for distributing the workload of a large *CScout*
analysis across multiple scripts for efficiency or manageability.
Split scripts can be used to speed-up an initial check
of the processing with \fCcscout -c
or to create a set of *SQLite* database files that
are then merged with *csmerge*(1).

The input is read from the specified file(s),
and the output files are written into the current working directory.
The input file(s) must have been created with *csmake*(1).
Each output script is named using a zero-padded sequence,
starting with *file-0000.cs*.

## Options

.BR -s ", " --shards " " N
Specifies the number of output scripts to generate.
The input processing script is divided into *N* approximately equal parts.
Each output script contains the appropriate subset of the input commands to
ensure no loss of information during the split.
Compilation units from multiple inter-dependent projects
(e.g. different configurations of the same executable)
are distributed to the scripts so as to maximize locality of reference.
.BR -h ", " --help
Display a brief usage message and exit.

## Example

To split an input processing script into 4 smaller scripts:

.DS
.ft C
.nf
cssplit -s 4  make.cs
.ft P
.fi
.DE

This will create the following files in the current directory:

.DS
.ft C
.nf
file-0000.cs
file-0001.cs
file-0002.cs
file-0003.cs
.ft P
.fi
.DE

Each file contains a portion of the original processing script.

## See also

*cscout*(1),
*cscut*(1),
*csmake*(1),
*csmerge*(1),
*python*(1)

## Author

(C) Copyright 2024-2025 Diomidis Spinellis.