Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
 127  MAXDIGIT        0     0       1   0     0     0   0     0  0    0  0  0  0   0   1
 165  used            0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 317  FILE            0     0       1   0     0     0   0     0  0    0  0  0  0   0   0
 463  x               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 617  main            1      0       0  0     1      0   0     0  0    0  0  1   0   1    0
 799  qqq             0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
1247  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
4163  __DATE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
4611  __TIME__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
5003  __FILE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
5409  __LINE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
5675  __STDC__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
7495  _cscout_dummy1  1      0       0  0     1      0   0     0  0    0  0  1   1    0   0
Table: Tokens
FID  FOFFSET   EID
  2      297  4163
  2      329  4611
  2      357  5003
  2      386  5409
  2      405  5675
  2      471   617
  2      535  7495
  2      558  7495
  4        7   107
  4       22   317
  4       55   317
  4       92   317
  5       11   165
  7        8   127
  7       32   463
  7       43   617
  7       56   799
  7       88  1247
Table: Rest
FID  FOFFSET  CODE
  2      287  \u0000a\u0000a#define 
  2      305    
  2      320  \u0000a#define 
  2      337   
  2      348  \u0000a#define 
  2      365   
  2      377  \u0000a#define 
  2      394   1\u0000a#define 
  2      413   1\u0000a\u0000a
  2      466  \u0000aint 
  2      475  ();\u0000a
  2      522  \u0000astatic void 
  2      549  (void) { 
  2      572  (); }\u0000a
  3      152  \u0000a\u0000a\u0000a#pragma includepath 
  3      195  \u0000a
  3      239  \u0000astatic void _cscout_dummy2(void) { _cscout_dummy2(); }\u0000a
  4        0  #ifdef 
  4       11  \u0000d\u0000a#define  
  4       26   
  4       38  \u0000d\u0000a#else\u0000d\u0000a#define 
  4       59   
  4       71  \u0000d\u0000a#endif\u0000d\u0000a\u0000d\u0000a#include 
  4       96  \u0000d\u0000a\u0000d\u0000a
  5        0  extern int 
  5       15  ;\u0000d\u0000a
  7        0  #define 
  7       16   11\u0000d\u0000aextern int 
  7       33  ;\u0000d\u0000aextern 
  7       47  ();\u0000d\u0000aint 
  7       59  ;\u0000d\u0000a\u0000d\u0000a
  7       86  \u0000d\u0000a
  7       91  () {\u0000d\u0000a	
  7      116  }\u0000d\u0000a\u0000d\u0000a
Table: Projects
PID  NAME
 16  unspecified
 17  Prj1
 18  Prj2
Table: IdProj
 EID  PID
 165   17
 317   17
 617   17
4163   17
4611   17
5003   17
5409   17
5675   17
7495   17
 127   18
 317   18
 463   18
 617   18
 799   18
1247   18
4163   18
4611   18
5003   18
5409   18
5675   18
7495   18
Table: Files
FID  NAME                 RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
  2  host-defs.h          1     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h          1     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c02-include_sites.c  0    100          0      23          0          0      8          26        2       0             6        1          0          1        19        1        1           0           0           0      0      0           0         0      0         0         1
  5  unused2.h            0     18          0       4          0          0      1          17        0       0             0        0          0          0         4        4        1           0           0           0      1      0           0         0      0         0         0
  6  unused1.h            0      0          0       0          0          0      0           0        0       0             0        0          0          0         0        0        1           0           0           0      0      0           0         0      0         0         0
  7  prj2.c               0    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
Table: FileProj
FID  PID
  2   17
  3   17
  4   17
  5   17
  1   18
  2   18
  3   18
  4   18
  6   18
  7   18
Table: Definers
PID  CUID  BASEFILEID  DEFINERID
 18     4           4          1
 18     7           7          2
Table: Includers
PID  CUID  BASEFILEID  INCLUDERID
 17     2           2           1
 17     4           3           1
 17     4           4           1
 17     4           5           4
 18     2           2           1
 18     2           2           1
 18     4           3           1
 18     4           4           1
 18     4           6           4
 18     7           3           1
 18     7           7           1
Table: Providers
PID  CUID  PROVIDERID
 17     2           2
 18     2           2
 18     2           2
 18     7           7
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
 18     7           7          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
1247  foo             0    1     1      0         7       88      0
6599  main            0    0    1      0         2      471      0
7495  _cscout_dummy1  0    1     1      1          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
      1247     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      16       0        0       0      0       0       1       1       1   0.0  0.0  0.0         7            95       7         117
      7495     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1       8       0        0       0      1       1       1       1       1   1.0  0.0  1.0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
      1247        0  1247
      6599        0   617
      7495        0  7495
Table: Fcalls
SOURCEID  DESTID
    7495    7495
Done
