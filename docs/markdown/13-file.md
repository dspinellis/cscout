# File Elements

Although some of the file queries operate on identifier properties,
all file queries produce file-list data as their result.
Clicking on an element of a file list leads you to a page
with a summary of the file.

File's directory

### Listings

-  Source code
-  Source code with unprocessed regions marked
-  Source code with identifier hyperlinks

-  Source code with hyperlinks to project-global writable identifiers
-  Source code with hyperlinks to function and macro declarations
-  Edit the file

### Functions

-  Defined project-scoped functions
-  Defined file-scoped functions

-  Function and macro call graph

### File Dependencies

-  Graph of files that depend on this file at compile time: writable, all-  Graph of files on which this file depends at compile time: writable, all-  Graph of files whose functions this file calls (control dependency): writable, all-  Graph of files calling this file's functions (control dependency): writable, all-  Graph of files whose global variables this file accesses (data dependency): writable, all-  Graph of files accessing this file's global variables (data dependency): writable, all

### Include Files

-  Writable files that this file directly includes
-  All files that this file includes
-  Include graph of all included files-  Include graph of writable included files-  Include graph of all including files-  All writable files that this file must include

-  Unused directly included files
-  Files including this file

### Metrics

| Metric | Value |
| --- | --- |
| Number of characters | 5155 |
| Number of comment characters | 1878 |
| Number of space characters | 754 |
| Number of line comments | 0 |
| Number of block comments | 27 |
| Number of lines | 190 |
| Maximum number of characters in a line | 107 |
| Number of character strings | 25 |
| Number of unprocessed lines | 0 |
| Number of C preprocessor directives | 9 |
| Number of processed C preprocessor conditionals (ifdef, if, elif) | 0 |
| Number of defined C preprocessor function-like macros | 0 |
| Number of defined C preprocessor object-like macros | 1 |
| Number of preprocessed tokens | 893 |
| Number of compiled tokens | 921 |
| Number of copies of the file | 1 |
| Number of statements | 123 |
| Number of defined project-scope functions | 3 |
| Number of defined file-scope (static) functions | 0 |
| Number of defined project-scope variables | 13 |
| Number of defined file-scope (static) variables | 0 |
| Number of complete aggregate (struct/union) declarations | 0 |
| Number of declared aggregate (struct/union) members | 0 |
| Number of complete enumeration declarations | 0 |
| Number of declared enumeration elements | 0 |
| Number of directly included files | 8 |

Main page
 — Web: Home
Manual
  

---
CScout

| Metric | Value |
| --- | --- |
| Number of characters | 5155 |
| Number of comment characters | 1878 |
| Number of space characters | 754 |
| Number of line comments | 0 |
| Number of block comments | 27 |
| Number of lines | 190 |
| Maximum number of characters in a line | 107 |
| Number of character strings | 25 |
| Number of unprocessed lines | 0 |
| Number of C preprocessor directives | 9 |
| Number of processed C preprocessor conditionals (ifdef, if, elif) | 0 |
| Number of defined C preprocessor function-like macros | 0 |
| Number of defined C preprocessor object-like macros | 1 |
| Number of preprocessed tokens | 893 |
| Number of compiled tokens | 921 |
| Number of copies of the file | 1 |
| Number of statements | 123 |
| Number of defined project-scope functions | 3 |
| Number of defined file-scope (static) functions | 0 |
| Number of defined project-scope variables | 13 |
| Number of defined file-scope (static) variables | 0 |
| Number of complete aggregate (struct/union) declarations | 0 |
| Number of declared aggregate (struct/union) members | 0 |
| Number of complete enumeration declarations | 0 |
| Number of declared enumeration elements | 0 |
| Number of directly included files | 8 |
| Number of directly included files | 8 | 8 |

The page starts with
the projects using this file,
whether the file also exists as an exact duplicate in other locations,
and a link to browse within *CScout* the directory where the file
is located.
Four subsequent sections provide links for

-  viewing the file's source code,

-  examining the functions defined in the file,

-  listing various file dependencies, and

-  exploring the file's include relationships.

The page ends with representative metrics for the given file.

## Source Code Views

You can view a file's source code in five different forms:

1.  The plain source code, will only provide you the file's code text
1.  The source code with unprocessed regions marked, will enable you
to see which parts of the file was not processed due to conditional compilation
instructions.
You may want to use the marked parts as a guide to construct a
more inclusive workspace definition (perhaps by processing the
project multiple times, with different preprocessor options).

| 360 #if defined(__GNUC__) && defined(__STDC__)   361 static __inline int __sputc(int _c, FILE *_p) {   362         if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))    363                 return (*_p->_p++ = _c);   364         else   365                 return (__swbuf(_c, _p));   366 }    367 #else   368 /*   369  * This has been tuned to generate reasonable code on the vax using pcc.    370  */   371 #define __sputc(c, p) \   372         (--(p)->_w < 0 ? \   373                 (p)->_w >= (p)->_lbfsize ? \    374                         (*(p)->_p = (c)), *(p)->_p != '\n' ? \   375                                 (int)*(p)->_p++ : \    376                                 __swbuf('\n', p) : \   377                         __swbuf((int)(c), p) : \   378                 (*(p)->_p = (c), (int)*(p)->_p++))    379 #endif   380 |
| --- |

1.  Source code with identifier hyperlinks, will provide you with
a page of the file's code text where each identifier is represented as
a hyperlink leading to the identifier's page.
The following is a representative example.

copy_fifo(from_stat, exists)  

        struct stat *from_stat;  

        int exists;  

{  

        if (exists && unlink(to.p_path)) {  

                warn("unlink: %s", to.p_path);  

                return (1);  

        }  

        if (mkfifo(to.p_path, from_stat->st_mode)) {  

                warn("mkfifo: %s", to.p_path);  

                return (1);  

        }  

        return (pflag ? setfile(from_stat, 0) : 0);  

}  

  

`

| int |
| --- |

1.  As the above display can be overwhelming, you may prefer
to browse the source code with hyperlinks only to project-global writable
identifiers, which are typically the most important identifiers.
Consider again how the above example would be displayed:

copy_fifo(from_stat, exists)  

        struct stat *from_stat;  

        int exists;  

{  

        if (exists && unlink(to.p_path)) {  

                warn("unlink: %s", to.p_path);  

                return (1);  

        }  

        if (mkfifo(to.p_path, from_stat->st_mode)) {  

                warn("mkfifo: %s", to.p_path);  

                return (1);  

        }  

        return (pflag ? setfile(from_stat, 0) : 0);  

}  

| int |
| --- |

1.  Source code with hyperlinks to function and macro declarations
provides you hyperlinks to the *function* pages for each
function declaration (implicit or explict) and macro definition.
Again, here is an example:
digittoint __P((int));  

<a id="86"></a>int     isascii __P((int));  

<a id="87"></a>int     isblank __P((int));  

<a id="88"></a>int     ishexnumber __P((int));  

<a id="89"></a>int     isideogram __P((int));  

<a id="90"></a>int     isnumber __P((int));  

<a id="91"></a>int     isphonogram __P((int));  

<a id="92"></a>int     isrune __P((int));  

<a id="93"></a>int     isspecial __P((int));  

<a id="94"></a>int     toascii __P((int));  

<a id="95"></a>#endif  

<a id="96"></a>__END_DECLS  

<a id="97"></a>  

<a id="98"></a>#define __istype(c,f)    (!!__maskrune((c),(f)))  

<a id="99"></a>  

<a id="100"></a>#define isalnum(c)       __istype((c), _CTYPE_A|_CTYPE_D)  

<a id="101"></a>#define isalpha(c)       __istype((c), _CTYPE_A)  

<a id="102"></a>#define iscntrl(c)       __istype((c), _CTYPE_C)  

<a id="103"></a>#define isdigit(c)       __isctype((c), _CTYPE_D) /* ANSI -- locale independent */  

<a id="104"></a>#define isgraph(c)       __istype((c), _CTYPE_G)  

<a id="105"></a>#define islower(c)       __istype((c), _CTYPE_L)  

<a id="106"></a>#define isprint(c)       __istype((c), _CTYPE_R)  

<a id="107"></a>#define ispunct(c)       __istype((c), _CTYPE_P)  

<a id="108"></a>#define isspace(c)       __istype((c), _CTYPE_S)  

<a id="109"></a>#define isupper(c)       __istype((c), _CTYPE_U)  

<a id="110"></a>#define isxdigit(c)      __isctype((c), _CTYPE_X) /* ANSI -- locale independent */  

<a id="111"></a>#define tolower(c)       __tolower(c)  

<a id="112"></a>#define toupper(c)       __toupper(c)  

<a id="113"></a>  

`

| #if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE) int |
| --- |

Finally, you can also choose to launch your editor on the file.
The way the editor is launched is specified in *CScout*'s
options page.

## File Metrics 

File metrics produces a summary of the workspace's file-based
metrics like the following:

| Metric | Total | Min | Max | Avg |
| --- | --- | --- | --- | --- |
| Number of characters | 157268 | 1923 | 43297 | 12097.5 |
| Number of comment characters | 30152 | 0 | 6307 | 2319.38 |
| Number of space characters | 28707 | 298 | 8735 | 2208.23 |
| Number of line comments | 0 | 0 | 0 | 0 |
| Number of block comments | 760 | 0 | 190 | 58.4615 |
| Number of lines | 6432 | 100 | 1913 | 494.769 |
| Maximum number of characters in a line | 1054 | 24 | 107 | 81.0769 |
| Number of character strings | 684 | 0 | 154 | 52.6154 |
| Number of unprocessed lines | 12 | 0 | 8 | 0.923077 |
| Number of C preprocessor directives | 274 | 0 | 92 | 21.0769 |
| Number of processed C preprocessor conditionals (ifdef, if, elif) | 6 | 0 | 3 | 0.461538 |
| Number of defined C preprocessor function-like macros | 30 | 0 | 22 | 2.30769 |
| Number of defined C preprocessor object-like macros | 161 | 0 | 92 | 12.3846 |
| Number of preprocessed tokens | 39529 | 393 | 12189 | 3040.69 |
| Number of compiled tokens | 44119 | 25 | 14020 | 3393.77 |
| Number of copies of the file | 13 | 1 | 1 | 1 |
| Number of statements | 4293 | 0 | 1589 | 330.231 |
| Number of defined project-scope functions | 168 | 0 | 51 | 12.9231 |
| Number of defined file-scope (static) functions | 2 | 0 | 1 | 0.153846 |
| Number of defined project-scope variables | 149 | 1 | 36 | 11.4615 |
| Number of defined file-scope (static) variables | 109 | 0 | 92 | 8.38462 |
| Number of complete aggregate (struct/union) declarations | 12 | 0 | 6 | 0.923077 |
| Number of declared aggregate (struct/union) members | 56 | 0 | 32 | 4.30769 |
| Number of complete enumeration declarations | 0 | 0 | 0 | 0 |
| Number of declared enumeration elements | 0 | 0 | 0 | 0 |
| Number of directly included files | 62 | 0 | 9 | 4.76923 |

### Read-only Files

Number of elements: 15

| Metric | Total | Min | Max | Avg |
| --- | --- | --- | --- | --- |
| Number of characters | 43094 | 227 | 9876 | 2872.93 |
| Number of comment characters | 26967 | 107 | 5695 | 1797.8 |
| Number of space characters | 3179 | 13 | 948 | 211.933 |
| Number of line comments | 12 | 0 | 12 | 0.8 |
| Number of block comments | 128 | 0 | 60 | 8.53333 |
| Number of lines | 1198 | 13 | 275 | 79.8667 |
| Maximum number of characters in a line | 1073 | 48 | 85 | 71.5333 |
| Number of character strings | 62 | 0 | 58 | 4.13333 |
| Number of unprocessed lines | 17 | 0 | 5 | 1.13333 |
| Number of C preprocessor directives | 288 | 1 | 102 | 19.2 |
| Number of processed C preprocessor conditionals (ifdef, if, elif) | 27 | 0 | 6 | 1.8 |
| Number of defined C preprocessor function-like macros | 30 | 0 | 13 | 2 |
| Number of defined C preprocessor object-like macros | 87 | 0 | 31 | 5.8 |
| Number of preprocessed tokens | 3461 | 16 | 1068 | 230.733 |
| Number of compiled tokens | 1779 | 0 | 602 | 118.6 |
| Number of copies of the file | 15 | 1 | 1 | 1 |
| Number of statements | 0 | 0 | 0 | 0 |
| Number of defined project-scope functions | 0 | 0 | 0 | 0 |
| Number of defined file-scope (static) functions | 0 | 0 | 0 | 0 |
| Number of defined project-scope variables | 6 | 0 | 3 | 0.4 |
| Number of defined file-scope (static) variables | 0 | 0 | 0 | 0 |
| Number of complete aggregate (struct/union) declarations | 7 | 0 | 3 | 0.466667 |
| Number of declared aggregate (struct/union) members | 56 | 0 | 23 | 3.73333 |
| Number of complete enumeration declarations | 0 | 0 | 0 | 0 |
| Number of declared enumeration elements | 0 | 0 | 0 | 0 |
| Number of directly included files | 24 | 0 | 20 | 1.6 |

Main page
 — Web: Home
Manual
  

---
CScout

| Metric | Total | Min | Max | Avg |
| --- | --- | --- | --- | --- |
| Number of characters | 43094 | 227 | 9876 | 2872.93 |
| Number of comment characters | 26967 | 107 | 5695 | 1797.8 |
| Number of space characters | 3179 | 13 | 948 | 211.933 |
| Number of line comments | 12 | 0 | 12 | 0.8 |
| Number of block comments | 128 | 0 | 60 | 8.53333 |
| Number of lines | 1198 | 13 | 275 | 79.8667 |
| Maximum number of characters in a line | 1073 | 48 | 85 | 71.5333 |
| Number of character strings | 62 | 0 | 58 | 4.13333 |
| Number of unprocessed lines | 17 | 0 | 5 | 1.13333 |
| Number of C preprocessor directives | 288 | 1 | 102 | 19.2 |
| Number of processed C preprocessor conditionals (ifdef, if, elif) | 27 | 0 | 6 | 1.8 |
| Number of defined C preprocessor function-like macros | 30 | 0 | 13 | 2 |
| Number of defined C preprocessor object-like macros | 87 | 0 | 31 | 5.8 |
| Number of preprocessed tokens | 3461 | 16 | 1068 | 230.733 |
| Number of compiled tokens | 1779 | 0 | 602 | 118.6 |
| Number of copies of the file | 15 | 1 | 1 | 1 |
| Number of statements | 0 | 0 | 0 | 0 |
| Number of defined project-scope functions | 0 | 0 | 0 | 0 |
| Number of defined file-scope (static) functions | 0 | 0 | 0 | 0 |
| Number of defined project-scope variables | 6 | 0 | 3 | 0.4 |
| Number of defined file-scope (static) variables | 0 | 0 | 0 | 0 |
| Number of complete aggregate (struct/union) declarations | 7 | 0 | 3 | 0.466667 |
| Number of declared aggregate (struct/union) members | 56 | 0 | 23 | 3.73333 |
| Number of complete enumeration declarations | 0 | 0 | 0 | 0 |
| Number of declared enumeration elements | 0 | 0 | 0 | 0 |
| Number of directly included files | 24 | 0 | 20 | 1.6 |
| Number of directly included files | 24 | 0 | 20 | 1.6 | 1.6 |

## All files 

The "All files" link will list all the project's files, including
source files, and directly and indirectly included files.
You can use this list to create a "bill of materials" for the files your
workspace requires to compile.
The following is an example of the output:
/home/dds/src/cscout/cscout_defs.h
1. /home/dds/src/cscout/cscout_incs.h
1. /usr/home/dds/src/cscout/bin.c
1. /usr/include/ctype.h
1. /usr/include/err.h
1. /usr/include/errno.h
1. /usr/include/fcntl.h
1. /usr/include/fts.h
1. /usr/include/limits.h
1. /usr/include/locale.h
1. /usr/include/machine/ansi.h
1. /usr/include/machine/endian.h
1. /usr/include/machine/limits.h
1. /usr/include/machine/param.h
1. /usr/include/machine/signal.h
1. /usr/include/machine/trap.h
1. /usr/include/machine/types.h
1. /usr/include/machine/ucontext.h
1. /usr/include/runetype.h
1. /usr/include/stdio.h
1. /usr/include/stdlib.h
1. /usr/include/string.h
1. /usr/include/sys/_posix.h
1. /usr/include/sys/cdefs.h
1. /usr/include/sys/inttypes.h
1. /usr/include/sys/param.h
1. /usr/include/sys/signal.h
1. /usr/include/sys/stat.h
1. /usr/include/sys/syslimits.h
1. /usr/include/sys/time.h
1. /usr/include/sys/types.h
1. /usr/include/sys/ucontext.h
1. /usr/include/sys/unistd.h
1. /usr/include/sysexits.h
1. /usr/include/syslog.h
1. /usr/include/time.h
1. /usr/include/unistd.h
1. /vol/src/bin/cp/cp.c
1. /vol/src/bin/cp/extern.h
1. /vol/src/bin/cp/utils.c
1. /vol/src/bin/date/date.c
1. /vol/src/bin/date/extern.h
1. /vol/src/bin/date/vary.h
1. /vol/src/bin/echo/echo.c

You can bookmark this page to save the respective query

Main page
  

---
CScout 1.6 - 2003/06/04 15:14:51

| All Files |
| --- |

## Read-only files 

The "Read-only files" link will typically show you the system files your
project used.
The following output was generated using the "Show file lists with file name in context" option.

ctype.h

err.h

errno.h

fcntl.h

fts.h

limits.h

locale.h

ansi.h

endian.h

limits.h

param.h

signal.h

trap.h

types.h

ucontext.h

runetype.h

stdio.h

stdlib.h

string.h

_posix.h

cdefs.h

inttypes.h

param.h

signal.h

stat.h

syslimits.h

time.h

types.h

ucontext.h

unistd.h

sysexits.h

syslog.h

time.h

unistd.h

| Directory | File |
| --- | --- |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |

You can bookmark this page to save the respective query

Main page

 - Web: Home
Manual
  

---
CScout 2.0 - 2004/07/31 12:37:12

| Directory | File |
| --- | --- |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/machine/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/sys/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |
| /usr/include/ |  |  |

## Writable files 

Correspondingly the "Writable files" link will only show you all your 
workspace's source files:

/home/dds/src/cscout/cscout_defs.h
1. /home/dds/src/cscout/cscout_incs.h
1. /usr/home/dds/src/cscout/bin.c
1. /vol/src/bin/cp/cp.c
1. /vol/src/bin/cp/extern.h
1. /vol/src/bin/cp/utils.c
1. /vol/src/bin/date/date.c
1. /vol/src/bin/date/extern.h
1. /vol/src/bin/date/vary.h
1. /vol/src/bin/echo/echo.c

You can bookmark this page to save the respective query

Main page
  

---
CScout 1.6 - 2003/06/04 15:14:51

| Writable Files |
| --- |

## Files containing unused project-scoped writable identifiers 

The link 
``files containing unused project-scoped writable identifiers''
performs an identifier query, but lists as output files containing
matching identifiers.
Specifically, the link will produce a list of files containing
global (project-scoped) unused writable identifiers.
Modern compilers can detect unused block-local or even file-local
(`static`) identifiers, but
detecting global identifiers is more tricky, since it requires 
processing of all files that will be linked together.
The restriction to writable identifiers will filter-out noise
generated through the use of the system's library functions.

In our example, the following list is generated:

cp.cmarked source

date.cmarked source

| Directory | File |
| --- | --- |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/date/ |  |  |

You can bookmark this page to save the respective query

Main page
 - Web: Home

Manual
  

---
CScout 2.0 - 2004/07/31 12:37:12

| Directory | File |
| --- | --- |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/date/ |  |  |
| /vol/src/bin/date/ |  |  |  |

The output contains the path to each file, and a link that will
generate the file's source code with the offending identifiers
marked as hyperlinks.
You can use the ``marked source'' link to inspect the identifiers in the
context of their source code;
simply follow the link with your browser and press `tab` 
to go to each hyperlink.
In our example the identifier will appear as follows:

setthetime(fmt, p, jflag, nflag)  

        const char *fmt;  

        register const char *p;  

        int jflag, nflag;  

{  

        register struct tm *lt;  

        struct timeval tv;  

        const char *dot, *t;  

        int century;  

  

`

| void |
| --- |

(In our case the function `setthetime` is declared as
`static`, but not defined as such.)

## Files containing unused file-scoped writable identifiers 

The link 
``files containing unused file-scoped writable identifiers''
performs an identifier query, but lists as output files containing
matching identifiers.
Specifically, the link will produce a list of files containing
file-scoped  (`static`) unused writable identifiers.
Although some modern compilers can detect file-local
identifiers, they fail to detect macros and some types of
variable declarations.
The *CScout* query is more general and can be more reliable.
The restriction to writable identifiers will filter-out noise
generated through the use of the system's library functions.

In our example, the following list is generated:

cp.cmarked source

utils.cmarked source

date.cmarked source

echo.cmarked source

| Directory | File |
| --- | --- |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/date/ |  |  |
| /vol/src/bin/echo/ |  |  |

You can bookmark this page to save the respective query

Main page
 - Web: Home
Manual
  

---
CScout 2.0 - 2004/07/31 12:37:12

| Directory | File |
| --- | --- |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/cp/ |  |  |
| /vol/src/bin/date/ |  |  |
| /vol/src/bin/echo/ |  |  |
| /vol/src/bin/echo/ |  |  |  |

In our case all identifiers located were the
`copyright` and the `rcsid`
identifiers.

copyright[] =  

"@(#) Copyright (c) 1989, 1993\n\  

        The Regents of the University of California.  All rights reserved.\n";  

#endif /* not lint */  

  

#ifndef lint  

#if 0  

static char sccsid[] = "@(#)echo.c      8.1 (Berkeley) 5/31/93";  

#endif  

static const char rcsid[] =  

  "$FreeBSD: src/bin/echo/echo.c,v 1.8.2.1 2001/08/01 02:33:32 obrien Exp $";  

#endif /* not lint */  

  

`

| #ifndef lint static char const |
| --- |

Later on we will explain how an identifier query could have used a regular
expression to filter-out the noise generated by these two identifiers.

## Writable .c files without any statements 

The
``writable .c files without any statements''
will locate C files that do not contain any C statements.
You can use it to locate files that only contain variable definitions,
or files that are `#ifdef`'d out.

In our example,
the result set only contains the processing script
(the compiled workspace definition file).

/usr/home/dds/src/cscout/bin.c

You can bookmark this page to save the respective query

Main page
  

---
CScout 1.6 - 2003/06/04 15:14:51

| Writable .c Files Without Any Statments |
| --- |

The processing script (the compiled workspace definition file)
follows the C syntax,
but only contains preprocessor directives
(mostly *CScout*-specific `#pragma` commands)
to drive the *CScout*'s source code analysis.

## Writable files containing unprocessed lines 

The ``writable files containing unprocessed lines'' link will present you
C files containing lines that were skipped by the C preprocossor,
due to conditional compilation directives.
The files are ordered according to the number of unprocessed lines
(files with the largest number will appear on the top).

In our case the results are:

utils.c

cscout_defs.h

echo.c

date.c

cp.c

cscout_incs.h

| Directory | File | Number of unprocessed lines |
| --- | --- | --- |
| /vol/src/bin/cp/ |  | 30 |
| /home/dds/src/cscout/ |  | 3 |
| /vol/src/bin/echo/ |  | 2 |
| /vol/src/bin/date/ |  | 2 |
| /vol/src/bin/cp/ |  | 2 |
| /home/dds/src/cscout/ |  | 2 |

You can bookmark this page to save the respective query

Main page
 - Web: Home

Manual
  

---
CScout 2.0 - 2004/07/31 12:37:12

| Directory | File | Number of unprocessed lines |
| --- | --- | --- |
| /vol/src/bin/cp/ |  | 30 |
| /home/dds/src/cscout/ |  | 3 |
| /vol/src/bin/echo/ |  | 2 |
| /vol/src/bin/date/ |  | 2 |
| /vol/src/bin/cp/ |  | 2 |
| /home/dds/src/cscout/ |  | 2 |
| /home/dds/src/cscout/ |  | 2 | 2 |

Lines skipped by the C preprocessor can be detrimental to the analysis
and the refactoring you perform.
If those lines contain live code that will be used under some other
circumstances (a different platform, or different configuration options),
then any results you obtain may miss important data.
The list of files allows you to see if there are any large chunks of
code that *CScout* ignored.
If there are, think about specifying additional configuration options as
preprocessor variables.
If some configuration options are mutually exclusive you can process the same
source multiple times, with different preprocessor variables set.

## Writable files containing strings 

The ``writable files containing strings'' link will present you
C files containing C strings.
In some applications user-messages are not supposed to be put
in the source code, to aid localization efforts.
This file query can then help you locate files that contain
strings.

In our case the results are:

/home/dds/src/cscout/cscout_defs.h
1. /home/dds/src/cscout/cscout_incs.h
1. /usr/home/dds/src/cscout/bin.c
1. /vol/src/bin/cp/cp.c
1. /vol/src/bin/cp/utils.c
1. /vol/src/bin/date/date.c
1. /vol/src/bin/echo/echo.c

You can bookmark this page to save the respective query

Main page
  

---
CScout 1.6 - 2003/06/04 15:14:51

| Writable Files Containing Strings |
| --- |

## Writable .h files with #include directives 

Some coding conventions dictate against recursive `#include`
invocations.
This query can be used to find files that break such a guideline.
As usual, read-only system files are excluded; these typically
use recursive `#include` invocations as a matter of course.

In our example, the result is:

/vol/src/bin/cp/extern.h

You can bookmark this page to save the respective query

Main page
  

---
CScout 1.6 - 2003/06/04 15:14:51

| Writable .h Files With #include directives |
| --- |
