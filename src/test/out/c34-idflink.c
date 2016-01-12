Table: Ids
  EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
-----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
   91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
  129  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
  331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  619  b               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    TRUE    FALSE
  891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 1109  c               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
 1599  d               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 2329  e               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
 2679  f               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    TRUE    FALSE
 2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3449  bar             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3865  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 4055  __STDC__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 5355  _cscout_dummy1  TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
11943  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
16463  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
Table: Tokens
FID  FOFFSET    EID
---  -------  -----
  1     1194  11943
  1     1646  16463
  2      297   2975
  2      329   3295
  2      357   3575
  2      386   3865
  2      405   4055
  2      471    441
  2      535   5355
  2      558   5355
  4       12    129
  4       34    129
  4       61    619
  4       83    619
  4      110   1109
  4      132   1109
  4      159   1599
  4      181   1599
  4      232   2329
  4      247   2329
  4      267   2679
  4      289   2679
  4      328    891
  4      344   3449
  4      366   3449
  5        8     91
  5       32    331
  5       43    441
  5       56    571
  5       88    891
Table: Rest
FID  FOFFSET  CODE
---  -------  ---------------------------------------------------------------------------------------------------------
  1       20  #pragma echo 
  1       64  \u0000a
  1       85  #pragma echo 
  1      129  \u0000a#pragma echo 
  1      172  \u0000a#pragma pushd 
  1      195  \u0000a
  1      212  #pragma echo 
  1      252  \u0000a#pragma project 
  1      275  \u0000a#pragma block_enter\u0000a
  1      318  #pragma echo 
  1      364  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1      512  \u0000a#pragma includepath 
  1      567  \u0000a#include 
  1      644  \u0000a#pragma process 
  1      676  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1      748  \u0000a#pragma block_exit\u0000a#pragma echo 
  1      813  \u0000a
  1      830  #pragma echo 
  1      870  \u0000a#pragma project 
  1      893  \u0000a#pragma block_enter\u0000a
  1      936  #pragma echo 
  1      982  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1130  \u0000a#pragma includepath 
  1     1185  \u0000a#define 
  1     1198  \u0000a#include 
  1     1275  \u0000a#pragma process 
  1     1307  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1379  \u0000a
  1     1395  #pragma echo 
  1     1434  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1582  \u0000a#pragma includepath 
  1     1637  \u0000a#define 
  1     1650  \u0000a#include 
  1     1727  \u0000a#pragma process 
  1     1752  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1817  \u0000a#pragma block_exit\u0000a#pragma echo 
  1     1882  \u0000a#pragma echo 
  1     1924  \u0000a#pragma popd\u0000a#pragma echo 
  1     1987  \u0000a#pragma echo 
  1     2037  \u0000a
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
  4        0  static void 
  4       13  (void);\u0000d\u0000astatic void 
  4       35  (void) { }\u0000d\u0000a\u0000d\u0000aextern void 
  4       62  (void);\u0000d\u0000astatic void 
  4       84  (void) { }\u0000d\u0000a\u0000d\u0000astatic void 
  4      111  (void);\u0000d\u0000aextern void 
  4      133  (void) { }\u0000d\u0000a\u0000d\u0000aextern void 
  4      160  (void);\u0000d\u0000aextern void 
  4      182  (void) { }\u0000d\u0000a\u0000d\u0000a
  4      218  \u0000d\u0000astatic void 
  4      233  (void);\u0000d\u0000avoid 
  4      248  (void) { }\u0000d\u0000a\u0000d\u0000avoid 
  4      268  (void);\u0000d\u0000astatic void 
  4      290  (void) { }\u0000d\u0000a\u0000d\u0000a
  4      326  \u0000d\u0000a
  4      331  ()\u0000d\u0000a{\u0000d\u0000a	void 
  4      347  (void);\u0000d\u0000a}\u0000d\u0000a\u0000d\u0000avoid 
  4      369  (void) { }\u0000d\u0000a
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
  129   17
  441   17
  619   17
  891   17
 1109   17
 1599   17
 2329   17
 2679   17
 2975   17
 3295   17
 3449   17
 3575   17
 3865   17
 4055   17
 5355   17
   91   18
  129   18
  331   18
  441   18
  571   18
  619   18
  891   18
 1109   18
 1599   18
 2329   18
 2679   18
 2975   18
 3295   18
 3449   18
 3575   18
 3865   18
 4055   18
 5355   18
11943   18
16463   18
Table: Files
FID  NAME           RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  -------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  1  makecs.cs      TRUE    2038        110     131          7          0     60          83       31       0            50        0          0          2       178        0        1           0           0           0      0      0           0         0      0         0         3
  2  cscout_defs.h  TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  cscout_incs.h  TRUE     290        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c34-idflink.c  FALSE    381         36      91          0          2     26          24        0       0             0        0          0          0       106      107        1           0           3           5      0      0           0         0      0         0         0
  5  prj2.c         FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
 18     5           5          4
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
 18     2           2
 18     2           2
 18     5           5
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
---  ----  ----------  ---------  -------  ---
 18     5           5          2      471    4
 18     5           5          4      328    3
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 349  a               FALSE    TRUE     TRUE      TRUE          4       34      0
 839  b               FALSE    TRUE     TRUE      FALSE         4       83      0
 891  foo             FALSE    TRUE     TRUE      FALSE         5       88      0
1329  c               FALSE    TRUE     TRUE      TRUE          4      132      0
1819  d               FALSE    TRUE     TRUE      FALSE         4      181      0
2479  e               FALSE    TRUE     TRUE      TRUE          4      247      0
2899  f               FALSE    TRUE     TRUE      FALSE         4      289      0
3669  bar             FALSE    TRUE     TRUE      FALSE         4      366      0
4715  main            FALSE    FALSE    TRUE      FALSE         2      471      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  -----  -----  --------  ------------  ------  ----------
       349      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      10       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4            43       4          45
       839      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      12       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4            92       4          94
       891     22          0       7          0          0      2          17        0       0             0        0          0          0         7        7      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     1     0     0    1      1      0      0     1      20       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           336       4         357
      1329      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      13       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           141       4         143
      1819      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      15       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           190       4         192
      2479      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      16       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           256       4         258
      2899      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      18       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           298       4         300
      3669      3          0       2          0          0      0           0        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      22       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4           377       4         379
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      13       0        0       0      1       1       1       1       1   1.0E0  0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       349        0   129
       839        0   619
       891        0   891
      1329        0  1109
      1819        0  1599
      2479        0  2329
      2899        0  2679
      3669        0  3449
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
    5355    5355
Done
