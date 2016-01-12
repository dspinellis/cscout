Table: Ids
  EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
-----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
   73  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
  265  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  353  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  457  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  713  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
 2381  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 2637  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 2861  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3093  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3245  __STDC__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 4285  _cscout_dummy1  TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
 9219  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
12723  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
Table: Tokens
FID  FOFFSET    EID
---  -------  -----
  1     1152   9219
  1     1590  12723
  2      297   2381
  2      329   2637
  2      357   2861
  2      386   3093
  2      405   3245
  2      471    353
  2      535   4285
  2      558   4285
  4        8     73
  4       32    265
  4       43    353
  4       56    457
  4       88    713
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
  1      311  #pragma echo 
  1      350  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1      498  \u0000a#pragma includepath 
  1      553  \u0000a#include 
  1      630  \u0000a#pragma process 
  1      655  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1      720  \u0000a#pragma block_exit\u0000a#pragma echo 
  1      785  \u0000a
  1      802  #pragma echo 
  1      842  \u0000a#pragma project 
  1      865  \u0000a#pragma block_enter\u0000a
  1      901  #pragma echo 
  1      940  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1088  \u0000a#pragma includepath 
  1     1143  \u0000a#define 
  1     1156  \u0000a#include 
  1     1233  \u0000a#pragma process 
  1     1258  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1323  \u0000a
  1     1339  #pragma echo 
  1     1378  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1526  \u0000a#pragma includepath 
  1     1581  \u0000a#define 
  1     1594  \u0000a#include 
  1     1671  \u0000a#pragma process 
  1     1696  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1761  \u0000a#pragma block_exit\u0000a#pragma echo 
  1     1826  \u0000a#pragma echo 
  1     1868  \u0000a#pragma popd\u0000a#pragma echo 
  1     1931  \u0000a#pragma echo 
  1     1981  \u0000a
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
  4        0  #define 
  4       16   11\u0000d\u0000aextern int 
  4       33  ;\u0000d\u0000aextern 
  4       47  ();\u0000d\u0000aint 
  4       59  ;\u0000d\u0000a\u0000d\u0000a
  4       86  \u0000d\u0000a
  4       91  () {\u0000d\u0000a	
  4      116  }\u0000d\u0000a\u0000d\u0000a
Table: Projects
PID  NAME
---  -----------
 16  unspecified
 17  Prj1
 18  Prj2
Table: IdProj
  EID  PID
-----  ---
   73   17
  265   17
  353   17
  457   17
  713   17
 2381   17
 2637   17
 2861   17
 3093   17
 3245   17
 4285   17
   73   18
  265   18
  353   18
  457   18
  713   18
 2381   18
 2637   18
 2861   18
 3093   18
 3245   18
 4285   18
 9219   18
12723   18
Table: Files
FID  NAME           RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  -------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  1  makecs.cs      TRUE    1982         96     131          7          0     60          83       31       0            50        0          0          2       178        0        1           0           0           0      0      0           0         0      0         0         3
  2  cscout_defs.h  TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  cscout_incs.h  TRUE     290        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  prj2.c         FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
Table: Definers
PID  CUID  BASEFILEID  DEFINERID
---  ----  ----------  ---------
 17     4           4          2
 18     4           4          2
 18     4           4          2
Table: Includers
PID  CUID  BASEFILEID  INCLUDERID
---  ----  ----------  ----------
 17     2           2           1
 17     4           3           1
 17     4           4           1
 18     2           2           1
 18     2           2           1
 18     4           3           1
 18     4           3           1
 18     4           4           1
 18     4           4           1
Table: Providers
PID  CUID  PROVIDERID
---  ----  ----------
 17     2           2
 17     4           4
 18     2           2
 18     2           2
 18     4           4
 18     4           4
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
---  ----  ----------  ---------  -------  ---
 17     4           4          2      471    4
 18     4           4          2      471    4
 18     4           4          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 713  foo             FALSE    TRUE     TRUE      FALSE         4       88      0
3773  main            FALSE    FALSE    TRUE      FALSE         2      471      0
4285  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC   CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  -----  -----  --------  ------------  ------  ----------
       713     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      16       0        0       0      0       0       1       1       1   0.0E0  0.0E0  0.0E0         4            95       4         117
      4285     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      11       0        0       0      1       1       1       1       1   1.0E0  0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       713        0   713
      3773        0   353
      4285        0  4285
Table: Fcalls
SOURCEID  DESTID
--------  ------
    4285    4285
Done
