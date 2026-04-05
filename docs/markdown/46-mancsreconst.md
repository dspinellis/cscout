# The csreconst Command Manual Page


## Name

csreconst - Test the reconstitution of files from a CScout database

## Synopsis

**csreconst** -t [**-ckqs**] *database.db*
  
**csreconst** -f *file* [**-o**] [**-b** *line*] [**-e** *line*] ... *database.db*

## Description

*csreconst*(1) is a utility for testing the reconstitution of files
stored in a *CScout*(1) database or reconstituting specific files.

The program provides two modes of operation:
**-t**
Test the reconstitution of all files in the specified SQLite *database.db*.
Additional options can be specified to refine the behavior of the testing:
**-c**
Count the number of correct and incorrect files.
**-k**
Keep comparing after finding a difference.
**-q**
Run a quick diff without listing the full differences.
**-s**
Provide a summary of each file's reconstitution result.

**-f**
Reconstitute a specified *file* stored in SQLite *database.db*.
Additional options allow fine-grained control of the reconstitution:
**-b \fIline**
Reconstitute starting from the specified beginning line number.
**-e \fIline**
Reconstitute up to the specified ending line number.
**-o**
Prefix each reconstituted part with its file offset value.

## Examples

Test all files in a *CScout* database and provide a summary:

.DS
.ft C
.nf
csreconst -tckqs result.db
.ft P
.fi
.DE

Reconstitute the file \fCmain.c from a *CScout* database:

.DS
.ft C
.nf
csreconst -f main.c result.db
.ft P
.fi
.DE

Reconstitute file \fCmain.c from line 10 to line 50,
listing the offset of each part:

.DS
.ft C
.nf
csreconst -f main.c -b 10 -e 50 -o result.db
.ft P
.fi
.DE

## Exit Status

The exit code indicates the result of the comparison operation:
0
All files compare equal (no differences found).
1
Differences were found between the reconstituted and original files.

## See Also

*cscout*(1),
*csmake*(1)

## Author

(C) Copyright 2024 Diomidis Spinellis.