Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
  91  MAXDIGIT        0     0       1   0     0     0   0     0  0    0  0  0  0   0   1
 331  x               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 441  main            1      0       0  0     1      0   0     0  0    0  0  1   0   1    0
 571  qqq             0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 891  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    0
2975  __DATE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3295  __TIME__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3575  __FILE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
3865  __LINE__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
4055  __STDC__        1      0       1   0     0     0   0     0  0    0  0  0  0   0   1
5355  _cscout_dummy1  1      0       0  0     1      0   0     0  0    0  0  1   1    0   0
5579  getconsxy       0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
5819  xy              0     0       0  0     1      0   0     0  0    0  0  0  0   0   1
6029  getconsxy2      0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
6279  xy              0     0       0  0     1      0   0     0  0    0  0  0  0   0   1
6589  putconsxy       0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
6829  xy              0     0       0  0     1      0   0     0  0    0  0  0  0   0   1
7279  xy              0     0       0  0     1      0   0     0  0    0  0  0  0   0   1
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
  4      557  5579
  4      581  5819
  4      602  6029
  4      627  6279
  4      658  6589
  4      682  6829
  4      709   891
  4      727  7279
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
  4      550  \u0000a\u0000avoid 
  4      566  (unsigned char 
  4      583  [static 2]);\u0000a\u0000avoid 
  4      612  (unsigned char 
  4      629  [static 2][static 5]);\u0000a\u0000avoid 
  4      667  (unsigned char 
  4      684  [static const 2]);\u0000a\u0000avoid 
  4      712  (unsigned char 
  4      729  [const]);\u0000a
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
 441   17
 891   17
2975   17
3295   17
3575   17
3865   17
4055   17
5355   17
5579   17
5819   17
6029   17
6279   17
6589   17
6829   17
7279   17
  91   18
 331   18
 441   18
 571   18
 891   18
2975   18
3295   18
3575   18
3865   18
4055   18
5355   18
5579   18
5819   18
6029   18
6279   18
6589   18
6829   18
7279   18
Table: Files
FID  NAME                     RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
  2  host-defs.h              1     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h              1     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c47-array-static-decl.c  0    739        546      26          0          1     19          70        0       0             0        0          0          0        52       53        1           0           0           0      0      0           0         0      0         0         0
  5  prj2.c                   0    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
 18     5           5          4
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
 18     5           5          4      709    3
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
 891  foo             0    1     1      0         5       88      0
4715  main            0    0    1      0         2      471      0
5355  _cscout_dummy1  0    1     1      1          2      535      1
5579  getconsxy       0    0    1      0         4      557      0
6029  getconsxy2      0    0    1      0         4      602      0
6589  putconsxy       0    0    1      0         4      658      0
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      19       0        0       0      0       0       1       1       1   0.0  0.0  0.0         5            95       5         117
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      12       0        0       0      1       1       1       1       1   1.0  0.0  1.0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
       891        0   891
      4715        0   441
      5355        0  5355
      5579        0  5579
      6029        0  6029
      6589        0  6589
Table: Fcalls
SOURCEID  DESTID
    5355    5355
Done
