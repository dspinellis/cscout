Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
  79  foo             0     0       0  0     0     1    0     0  0    0  0  0  0   0   1
  91  MAXDIGIT        0     0       1   0     0     0   0     0  0    0  0  0  0   0   1
 199  a               0     0       0  0     0     0   1      0  0    0  0  0  0   0   1
 229  b               0     0       0  0     0     0   1      0  0    0  0  0  0   0   1
 331  x               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 349  bar             0     0       0  0     0     1    0     0  0    0  0  0  0   0   1
 441  main            1      0       0  0     1      0   0     0  0    0  0  1   0   1    0
 449  k               0     0       0  0     0     0   1      0  0    0  0  0  0   0   1
 499  l               0     0       0  0     0     0   1      0  0    0  0  0  0   0   1
 559  a               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 571  qqq             0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
 829  foo2            0     0       0  0     0     1    0     0  0    0  0  0  0   0   0
 891  foo             0     0       0  0     1      0   0     0  0    0  0  1   0   1    1
 959  X               0     0       0  0     0     0   1      0  0    0  0  0  0   0   0
 989  b               0     0       0  0     0     0   1      0  0    0  0  0  0   0   0
1109  bar2            0     0       0  0     0     1    0     0  0    0  0  0  0   0   1
1219  k               0     0       0  0     0     0   1      0  0    0  0  0  0   0   0
1269  l               0     0       0  0     0     0   1      0  0    0  0  0  0   0   0
1359  c               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
1889  d               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
2679  b               0     0       0  0     1      0   0     0  0    0  0  0  0   1    1
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
  4        7    79
  4       19   199
  4       22   229
  4       34   349
  4       44   449
  4       49   499
  4       55   559
  4       82   829
  4       95   959
  4       98   989
  4      110  1109
  4      121  1219
  4      126  1269
  4      135  1359
  4      142   959
  4      150   989
  4      158  1269
  4      166  1219
  4      177  1219
  4      188  1889
  4      195   959
  4      203   989
  4      211  1269
  4      225  1219
  4      242  1219
  4      262   829
  4      267  2679
  4      273   989
  4      281  1269
  4      286  1219
  4      294   959
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
  4        0  struct 
  4       10   {\u0000d\u0000a	int 
  4       20  , 
  4       23  ;\u0000d\u0000a	struct 
  4       37   { int 
  4       45  ; } 
  4       50  ;\u0000d\u0000a} 
  4       56   = {1, 2, {3}};\u0000d\u0000a\u0000d\u0000astruct 
  4       86   {\u0000d\u0000a	int 
  4       96  , 
  4       99  ;\u0000d\u0000a	struct 
  4      114   { int 
  4      122  ; } 
  4      127  [3];\u0000d\u0000a} 
  4      136   = { .
  4      143   = 1, .
  4      151   = 2, .
  4      159   = {{ .
  4      167   = 3}, { .
  4      178   = 4}}},\u0000d\u0000a
  4      189   = { .
  4      196   = 1, .
  4      204   = 2, .
  4      212   = {[0] = { .
  4      226   = 3}, [1] = { .
  4      243   = 4}}};\u0000d\u0000a\u0000d\u0000astruct 
  4      266   
  4      268   = {.
  4      274   = 1, .
  4      282  [2].
  4      287   = 3, .
  4      295   = 8};\u0000d\u0000a
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
  79   17
 199   17
 229   17
 349   17
 441   17
 449   17
 499   17
 559   17
 829   17
 959   17
 989   17
1109   17
1219   17
1269   17
1359   17
1889   17
2679   17
2975   17
3295   17
3575   17
3865   17
4055   17
5355   17
  79   18
  91   18
 199   18
 229   18
 331   18
 349   18
 441   18
 449   18
 499   18
 559   18
 571   18
 829   18
 891   18
 959   18
 989   18
1109   18
1219   18
1269   18
1359   18
1889   18
2679   18
2975   18
3295   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME               RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
  2  host-defs.h        1     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h        1     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c04-designators.c  0    303          0     106          0          0     12          64        0       0             0        0          0          0       151      152        1           0           0           0      4      0           4         8      0         0         0
  5  prj2.c             0    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      20       0        0       0      0       0       1       1       1   0.0  0.0  0.0         5            95       5         117
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      12       0        0       0      1       1       1       1       1   1.0  0.0  1.0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
       891        0   891
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
    5355    5355
Done
