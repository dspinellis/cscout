Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
  59  whole           0     0       0  0     1      0   0     0  0    0  0  1   0   1    0
  91  MAXDIGIT        0     0       1   0     0     0   0     0  0    0  0  0  0   0   1
 331  x               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 441  main            1      0       0  0     1      0   0     0  0    0  0  1   0   1    0
 571  qqq             0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 659  whole2          0     0       0  0     1      0   0     0  0    0  0  1   0   1    0
 891  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
1139  conc            0     0       1   0     0     0   0     0  0    0  0  0  0   0   0
1189  a               0     0       0  1      0     0   0     0  0    0  0  0  0   0   0
1219  b               0     0       0  1      0     0   0     0  0    0  0  0  0   0   0
1379  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    0
1409  bar             0     0       0  0     1      0   0     0  0    0  0  1   0   1    0
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
  4        5    59
  4       24    59
  4       44    59
  4       65   659
  4       86   659
  4      113  1139
  4      118  1189
  4      121  1219
  4      124  1189
  4      129  1219
  4      137  1379
  4      140  1409
  4      165  1139
  4      170  1379
  4      175  1409
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
  4        0  void 
  4       10  (void);\u0000d\u0000avoid 
  4       29  (void){}\u0000d\u0000avoid 
  4       49  (void);\u0000d\u0000a\u0000d\u0000avoid 
  4       71  (void){}\u0000d\u0000avoid 
  4       92  (void);\u0000d\u0000a\u0000d\u0000a\u0000d\u0000a#define 
  4      117  (
  4      119  , 
  4      122  ) 
  4      125   ## 
  4      130  \u0000d\u0000avoid 
  4      143  (void){}\u0000d\u0000aextern void 
  4      169  (
  4      173  , 
  4      178  )(void);\u0000d\u0000a\u0000d\u0000a\u0000d\u0000a
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
  59   17
 441   17
 659   17
1139   17
1189   17
1219   17
1379   17
1409   17
2975   17
3295   17
3575   17
3865   17
4055   17
5355   17
  59   18
  91   18
 331   18
 441   18
 571   18
 659   18
 891   18
1139   18
1189   18
1219   18
1379   18
1409   18
2975   18
3295   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME           RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
  2  host-defs.h    1     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h    1     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c09-concfun.c  0    192          0      40          0          0     13          34        0       0             1        0          1          0        64       47        1           0           3           0      0      0           0         0      0         0         0
  5  prj2.c         0    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
 18     2           2
 18     2           2
 18     5           5
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
 18     5           5          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
 249  whole           0    1     1      0         4       24      0
 659  whole2          0    1     1      0         4       65      0
 891  foo             0    1     1      0         5       88      0
1139  conc            1     1     0     1          4      113      0
1409  foobar          0    1     1      0         4      140      0
4715  main            0    0    1      0         2      471      0
5355  _cscout_dummy1  0    1     1      1          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
       249      2          0       1          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      11       0        0       0      0       0       1       1       1   0.0  0.0  0.0         4            36       4          37
       659      2          0       1          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      13       0        0       0      0       0       1       1       1   0.0  0.0  0.0         4            78       4          79
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      19       0        0       0      0       0       1       1       1   0.0  0.0  0.0         5            95       5         117
      1139     10          0       5          0          0      1           8        0       0             0        0          0          0         3        0      0    1     1        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0       0       2        0       0      0       0       1       1       1   0.0  0.0  0.0         4           123       4         132
      1409      2          0       1          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      16       0        0       0      0       0       1       1       1   0.0  0.0  0.0         4           150       4         151
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      11       0        0       0      1       1       1       1       1   1.0  0.0  1.0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
       249        0    59
       659        0   659
       891        0   891
      1139        0  1139
      1409        0  1379
      1409        1  1409
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
    5355    5355
Done
