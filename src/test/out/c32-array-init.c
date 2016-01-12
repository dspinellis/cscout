Table: Ids
  EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
-----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
   59  ti_statetbl     FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
   91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
  331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
 2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3865  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 4055  __STDC__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 5355  _cscout_dummy1  TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
12123  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
16703  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
Table: Tokens
FID  FOFFSET    EID
---  -------  -----
  1     1212  12123
  1     1670  16703
  2      297   2975
  2      329   3295
  2      357   3575
  2      386   3865
  2      405   4055
  2      471    441
  2      535   5355
  2      558   5355
  4        5     59
  5        8     91
  5       32    331
  5       43    441
  5       56    571
  5       88    891
Table: Rest
FID  FOFFSET  CODE
---  -------  ----------------------------------------------------------------------------------------------------------------------------------
  1       20  #pragma echo 
  1       64  \u0000a
  1       85  #pragma echo 
  1      129  \u0000a#pragma echo 
  1      172  \u0000a#pragma pushd 
  1      195  \u0000a
  1      212  #pragma echo 
  1      252  \u0000a#pragma project 
  1      275  \u0000a#pragma block_enter\u0000a
  1      321  #pragma echo 
  1      370  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1      518  \u0000a#pragma includepath 
  1      573  \u0000a#include 
  1      650  \u0000a#pragma process 
  1      685  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1      760  \u0000a#pragma block_exit\u0000a#pragma echo 
  1      825  \u0000a
  1      842  #pragma echo 
  1      882  \u0000a#pragma project 
  1      905  \u0000a#pragma block_enter\u0000a
  1      951  #pragma echo 
  1     1000  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1148  \u0000a#pragma includepath 
  1     1203  \u0000a#define 
  1     1216  \u0000a#include 
  1     1293  \u0000a#pragma process 
  1     1328  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1403  \u0000a
  1     1419  #pragma echo 
  1     1458  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1606  \u0000a#pragma includepath 
  1     1661  \u0000a#define 
  1     1674  \u0000a#include 
  1     1751  \u0000a#pragma process 
  1     1776  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1841  \u0000a#pragma block_exit\u0000a#pragma echo 
  1     1906  \u0000a#pragma echo 
  1     1948  \u0000a#pragma popd\u0000a#pragma echo 
  1     2011  \u0000a#pragma echo 
  1     2061  \u0000a
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
  3      190  \u0000a
  3      234  \u0000astatic void _cscout_dummy2(void) { _cscout_dummy2(); }\u0000a
  4        0  char 
  4       16  [28][17] = {\u0000d\u0000a{ 1, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 2, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 4, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 0, 3, 127, 3, 3, 127, 127, 7, 127, 127, 127, 6, 7, 9, 10, 11},\u0000d\u0000a{127, 127, 0, 127, 127, 6, 127, 127, 127, 127, 127, 127, 3, 127, 3, 3, 3},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 9, 127, 127, 127, 127, 3, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 3, 127, 127, 127, 127, 3, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 7, 127, 127, 127, 127, 7, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 5, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 8, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 12, 13, 127, 14, 15, 16, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 9, 127, 11, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 9, 127, 11, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 10, 127, 3, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 7, 127, 127, 127, 7, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 9, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 9, 10, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 9, 10, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 127, 127, 11, 3, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 3, 127, 127, 3, 3, 3, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 127, 127, 127, 127, 7, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 9, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a{127, 127, 127, 3, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},\u0000d\u0000a};\u0000d\u0000a
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
---  -----------
 16  unspecified
 17  Prj1
 18  Prj2
Table: IdProj
  EID  PID
-----  ---
   59   17
  441   17
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
  891   18
 2975   18
 3295   18
 3575   18
 3865   18
 4055   18
 5355   18
12123   18
16703   18
Table: Files
FID  NAME              RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  ----------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  1  makecs.cs         TRUE    2062        116     131          7          0     60          83       31       0            50        0          0          2       178        0        1           0           0           0      0      0           0         0      0         0         3
  2  cscout_defs.h     TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  cscout_incs.h     TRUE     290        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c32-array-init.c  FALSE   2394          0     512          0          0     30          86        0       0             0        0          0          0      1020     1021        1           0           0           0      1      0           0         0      0         0         0
  5  prj2.c            FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
Table: FileProj
FID  PID
---  ---
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
---  ----  ----------  ---------
 18     5           5          2
Table: Includers
PID  CUID  BASEFILEID  INCLUDERID
---  ----  ----------  ----------
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
---  ----  ----------
 17     2           2
 17     4           4
 18     2           2
 18     2           2
 18     4           4
 18     5           5
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
---  ----  ----------  ---------  -------  ---
 18     5           5          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 891  foo             FALSE    TRUE     TRUE      FALSE         5       88      0
4715  main            FALSE    FALSE    TRUE      FALSE         2      471      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  -----  -----  --------  ------------  ------  ----------
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      17       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         5            95       5         117
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1       9       0        0       0      1       1       1       1       1   1.0E0  0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       891        0   891
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
    5355    5355
Done
