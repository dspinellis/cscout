# The csmerge Command Manual Page


## Name

csmerge - merge multiple CScout database shards

## Synopsis

.B csmerge
[**-k**]
[**-l** *file*]
[**-T** *dir*]
*nfiles* *merged.db*

## Description

.B csmerge
merges a set of CScout SQLite database shards into a single database.
The program expects as input
.I nfiles
SQLite database files.
These are merged into the specified
.I merged.db
output database file.
The resulting file preserves the semantics of *CScout*
processing, including primary key assignments, foreign key references,
and token splits.
The number of input databases must be a power of two, e.g. 2, 4, 16, or 64.
Each input database is named using a zero-padded sequence,
starting with *file-0000.db*.
The databases are typically produced by *CScout* processing
a larger *CScout* file split into multiple parts
with *cssplit*(1).
.pp
Large temporary files are created during the merge and removed when finished,
unless instructed otherwise.
During the processing the commands *sqlite3*(1) and *cscout*(1)
are invoked to handle the merging.

## Options

.B -k
Preserve temporary files created during execution.
.BI -l " file"
Write logs to the specified file.  
The default log file is
.IR dbmerge.log .
.BI -T " dir"
Use
.I dir
as the temporary directory.
Defaults to the value of
.B TMPDIR
if set, otherwise
.IR /tmp .

## Exit Status

Returns 0 on success, nonzero on errors.

## Environment

.B TMPDIR
Temporary directory if
.BR -T
is not provided.
The temporary directory must have enough space to store the resulting database,
which will be about 30% larger than the total size of the combined database
files.

## Files

.I dbmerge.log
Default log output.

## Examples

Below is a complete example of splitting the processing of several
Linux kernel configurations into 64 chunks and then merging the results.
.ft C
.nf
# Create and use a space for the split data.
mkdir parallel
cd parallel
# Split configurations into multiple files.
cssplit -s 64 ../linux-*.cs
# Run CScout on all split files in parallel.
for f in file-*.cs ; do
  cscout -s sqlite $f | sqlite3 $f.db &
done
# Wait for the CScout jobs to finish.
while wait -n; do :; done
# Merge the results into a single database.
csmerge 64 ../merged.db
.fi
.ft P

## See also

*cscout*(1),
*cssplit*(1),
*csmake*(1),
*sqlite3*(1),
*mktemp*(1)

## Author

(C) Copyright 2025 Diomidis Spinellis.