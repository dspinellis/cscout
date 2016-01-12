Table: Ids
  EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
-----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
   91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
  159  __wait_queue    FALSE     FALSE       FALSE  FALSE     FALSE     TRUE    FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
  289  wait_queue_t    FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  TRUE     FALSE  FALSE  FALSE  TRUE    FALSE   FALSE
  331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  529  list_head       FALSE     FALSE       FALSE  FALSE     FALSE     TRUE    FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
  571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
  891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  919  next            FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
  989  prev            FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 1559  flags           FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 1779  private         FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 1999  func            FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 2319  task_list       FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 2829  name            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 3865  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 4055  __STDC__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 4129  name2           FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 5355  _cscout_dummy1  TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
12843  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
17663  PRJ2            TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
Table: Tokens
FID  FOFFSET    EID
---  -------  -----
  1     1284  12843
  1     1766  17663
  2      297   2975
  2      329   3295
  2      357   3575
  2      386   3865
  2      405   4055
  2      471    441
  2      535   5355
  2      558   5355
  4       15    159
  4       28    289
  4       52    529
  4       80    529
  4       91    919
  4       98    989
  4      118    159
  4      155   1559
  4      177   1779
  4      199   1999
  4      221    529
  4      231   2319
  4      251    891
  4      269    289
  4      282   2829
  4      301   1779
  4      334   1999
  4      357   2319
  4      398    289
  4      412   4129
  4      423   4129
  4      430   2319
  4      440    919
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
  1      333  #pragma echo 
  1      394  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1      542  \u0000a#pragma includepath 
  1      597  \u0000a#include 
  1      674  \u0000a#pragma process 
  1      721  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1      808  \u0000a#pragma block_exit\u0000a#pragma echo 
  1      873  \u0000a
  1      890  #pragma echo 
  1      930  \u0000a#pragma project 
  1      953  \u0000a#pragma block_enter\u0000a
  1     1011  #pragma echo 
  1     1072  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1220  \u0000a#pragma includepath 
  1     1275  \u0000a#define 
  1     1288  \u0000a#include 
  1     1365  \u0000a#pragma process 
  1     1412  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1499  \u0000a
  1     1515  #pragma echo 
  1     1554  \u0000a#pragma block_enter\u0000a#pragma clear_defines\u0000a#pragma clear_include\u0000a#pragma process 
  1     1702  \u0000a#pragma includepath 
  1     1757  \u0000a#define 
  1     1770  \u0000a#include 
  1     1847  \u0000a#pragma process 
  1     1872  \u0000a\u0000a#pragma block_exit\u0000a#pragma echo 
  1     1937  \u0000a#pragma block_exit\u0000a#pragma echo 
  1     2002  \u0000a#pragma echo 
  1     2044  \u0000a#pragma popd\u0000a#pragma echo 
  1     2107  \u0000a#pragma echo 
  1     2157  \u0000a
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
  4        0  typedef struct 
  4       27   
  4       40  ;\u0000d\u0000a\u0000d\u0000astruct 
  4       61   {\u0000d\u0000a        struct 
  4       89   *
  4       95  , *
  4      102  ;\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000astruct 
  4      130   {\u0000d\u0000a        unsigned int 
  4      160  ;\u0000d\u0000a        void *
  4      184  ;\u0000d\u0000a        int 
  4      203  ;\u0000d\u0000a        struct 
  4      230   
  4      240  ;\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000a\u0000d\u0000a
  4      254  ()\u0000d\u0000a{\u0000d\u0000a        
  4      281   
  4      286   = {\u0000d\u0000a        .
  4      308          = 
  4      322  ,\u0000d\u0000a        .
  4      338             = 0,\u0000d\u0000a	.
  4      366   = {0, 0}\u0000d\u0000a         };\u0000d\u0000a        
  4      410   *
  4      417  ;\u0000d\u0000a\u0000d\u0000a	
  4      428  ->
  4      439  .
  4      444   = 0;\u0000d\u0000a}\u0000d\u0000a
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
  159   17
  289   17
  441   17
  529   17
  891   17
  919   17
  989   17
 1559   17
 1779   17
 1999   17
 2319   17
 2829   17
 2975   17
 3295   17
 3575   17
 3865   17
 4055   17
 4129   17
 5355   17
   91   18
  159   18
  289   18
  331   18
  441   18
  529   18
  571   18
  891   18
  919   18
  989   18
 1559   18
 1779   18
 1999   18
 2319   18
 2829   18
 2975   18
 3295   18
 3575   18
 3865   18
 4055   18
 4129   18
 5355   18
12843   18
17663   18
Table: Files
FID  NAME                          RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  ----------------------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  1  makecs.cs                     TRUE    2158        140     131          7          0     60          83       31       0            50        0          0          2       178        0        1           0           0           0      0      0           0         0      0         0         3
  2  cscout_defs.h                 TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  cscout_incs.h                 TRUE     290        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c33-incomplete-initializer.c  FALSE    454          0     179          0          0     25          42        1       0             0        0          0          0        79       80        1           1           1           0      0      0           2         6      0         0         0
  5  prj2.c                        FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
 17     4           4
 18     2           2
 18     2           2
 18     4           4
 18     5           5
Table: IncTriggers
PID  CUID  BASEFILEID  DEFINERID  FOFFSET  LEN
---  ----  ----------  ---------  -------  ---
 18     5           5          2      471    4
 18     5           5          4      251    3
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 891  foo             FALSE    TRUE     TRUE      FALSE         5       88      0
4715  main            FALSE    FALSE    TRUE      FALSE         2      471      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -----  --------  ------------  ------  ----------
       891    194          0      92          0          0      9          32        1       0             0        0          0          0        37       37      3   11     4        4      0    0      0        0      0         0       0     0       0    0          0      0        0     0     2     0    5      0      1      0     3      16       0        0       0      0       0       1       1       1   0.0E0  69.1886E0  0.0E0         4           259       4         452
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1       9       0        0       0      1       1       1       1       1   1.0E0      0.0E0  1.0E0         2           557       2         577
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
