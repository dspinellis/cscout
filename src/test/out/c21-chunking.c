Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
  89  a               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
  91  MAXDIGIT        0     0       1   0     0     0   0     0  0    0  0  0  0   0   1
 239  k               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 331  x               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 441  main            1      0       0  0     1      0   0     0  0    0  0  1   0   1    0
 571  qqq             0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 891  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
1169  u               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
1539  c               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
1679  c2              0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
2975  __DATE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3295  __TIME__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3575  __FILE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3865  __LINE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
4055  __STDC__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
5355  _cscout_dummy1  1      0       0  0     1      0   0     0  0    0  0  1   1    0   0
Table: Tokens
FID  FOFFSET   EID
  2      297  2975
  2      329  3295
  2      357  3575
  2      386  3865
  2      405  4055
  2      471   441
  2      535  5355
  2      558  5355
  4        8    89
  4       23   239
  4      116  1169
  4      153  1539
  4      167  1679
  5        8    91
  5       32   331
  5       43   441
  5       56   571
  5       88   891
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
  4        0  int
  4        9  ;\u0000d\u0000a
  4       17  char *
  4       24  =
  4       53  ;\u0000d\u0000a
  4      107  \u0000d\u0000a char *
  4      117  =
  4      144  ;\u0000d\u0000a char 
  4      154  ='"';\u0000d\u0000a char 
  4      169  ='\'';\u0000d\u0000a
  5        0  #define 
  5       16   11\u0000d\u0000aextern int 
  5       33  ;\u0000d\u0000aextern 
  5       47  ();\u0000d\u0000aint 
  5       59  ;\u0000d\u0000a\u0000d\u0000a
  5       86  \u0000d\u0000a
  5       91  () {\u0000d\u0000a	
  5      116  }\u0000d\u0000a\u0000d\u0000a
Table: Projects
PID  NAME
 16  unspecified
 17  Prj1
 18  Prj2
Table: IdProj
 EID  PID
  89   17
 239   17
 441   17
1169   17
1539   17
1679   17
2975   17
3295   17
3575   17
3865   17
4055   17
5355   17
  89   18
  91   18
 239   18
 331   18
 441   18
 571   18
 891   18
1169   18
1539   18
1679   18
2975   18
3295   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME            RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
  2  host-defs.h     1     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h     1     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c21-chunking.c  0    177         66      19          1          4     10          38        3       0             0        0          0          0        26       27        1           0           0           0      5      0           0         0      0         0         0
  5  prj2.c          0    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
Table: FileProj
FID  PID
  2   17
  3   17
  4   17
  1   18
  2   18
  3   18
  4   18
  5   18
Table: Definers
PID  CUID  BASEFILEID  DEFINERID
 18     5           5          2
Table: Includers
PID  CUID  BASEFILEID  INCLUDERID
 17     2           2           1
 17     4           3           1
 17     4           4           1
 18     2           2           1
 18     2           2           1
 18     4           3           1
 18     4           4           1
 18     5           3           1
 18     5           5           1
Table: Providers
PID  CUID  PROVIDERID
 17     2           2
 17     4           4
 18     2           2
 18     2           2
 18     4           4
 18     5           5
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
 18     5           5          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
 891  foo             0    1     1      0         5       88      0
4715  main            0    0    1      0         2      471      0
5355  _cscout_dummy1  0    1     1      1          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      21       0        0       0      0       0       1       1       1   0.0  0.0  0.0         5            95       5         117
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      13       0        0       0      1       1       1       1       1   1.0  0.0  1.0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
       891        0   891
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
    5355    5355
Done
