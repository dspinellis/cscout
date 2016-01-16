Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
  79  foo             FALSE     FALSE       FALSE  FALSE     TRUE      TRUE    TRUE      TRUE   FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 459  bar             FALSE     FALSE       FALSE  FALSE     FALSE     TRUE    FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 849  getpart         FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 929  tag             FALSE     FALSE       FALSE  TRUE      FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 979  name            FALSE     FALSE       FALSE  TRUE      FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1199  p               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1389  getfoo          FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1459  var             FALSE     FALSE       FALSE  TRUE      FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1699  get             FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1739  name            FALSE     FALSE       FALSE  TRUE      FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2259  conditional     FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2379  x               FALSE     FALSE       FALSE  TRUE      FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3309  f               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
3539  b               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3865  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
4055  __STDC__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
5355  _cscout_dummy1  TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
Table: Tokens
FID  FOFFSET   EID
---  -------  ----
  2      297  2975
  2      329  3295
  2      357  3575
  2      386  3865
  2      405  4055
  2      471   441
  2      535  5355
  2      558  5355
  4        7    79
  4       26    79
  4       45   459
  4       64    79
  4       84   849
  4       92   929
  4       97   979
  4      113   929
  4      119  1199
  4      123   979
  4      138  1389
  4      145  1459
  4      151  1459
  4      155    79
  4      169  1699
  4      173  1739
  4      180  1739
  4      199  1739
  4      206  1199
  4      210  1739
  4      225  2259
  4      237  2379
  4      249  2379
  4      260  2379
  4      270  2379
  4      294    79
  4      304  1199
  4      326    79
  4      330  3309
  4      349   459
  4      353  3539
  4      359    79
  4      377  1199
  4      382   849
  4      390    79
  4      395    79
  4      425   849
  4      433   459
  4      438    79
  4      462  1389
  4      469  3309
  4      497  1699
  4      501    79
  4      525  1389
  4      532  3539
  4      553  2259
  4      565    79
  5        8    91
  5       32   331
  5       43   441
  5       56   571
  5       88    79
Table: Rest
FID  FOFFSET  CODE
---  -------  ------------------------------------------------------------------------------------------------
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
  4        0  struct 
  4       10   {\u0000d\u0000a        int 
  4       29  ;\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000astruct 
  4       48   {\u0000d\u0000a        int 
  4       67  ;\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000a#define 
  4       91  (
  4       95  , 
  4      101  ) (((struct 
  4      116   *)
  4      120  )->
  4      127  )\u0000d\u0000a#define 
  4      144  (
  4      148  ) (
  4      154  .
  4      158  )\u0000d\u0000a#define 
  4      172  (
  4      177  ) (
  4      184  (0) + ((struct 
  4      203   *)
  4      207  )->
  4      214  )\u0000d\u0000a#define 
  4      236  (
  4      238  ) do {if (!
  4      250  (0)) goto 
  4      261  ; return 
  4      271  (0);} while(0)\u0000d\u0000a\u0000d\u0000aint\u0000d\u0000a
  4      297  (void *
  4      305  )\u0000d\u0000a{\u0000d\u0000a        struct 
  4      329   
  4      331  ;\u0000d\u0000a        struct 
  4      352   
  4      354  ;\u0000d\u0000a\u0000d\u0000a
  4      362  :\u0000d\u0000a        if (
  4      378   && 
  4      389  (
  4      393  , 
  4      398  ))\u0000d\u0000a                return 
  4      432  (
  4      436  , 
  4      441  );\u0000d\u0000a        else if (
  4      468  (
  4      470  ))\u0000d\u0000a                return 
  4      500  (
  4      504  );\u0000d\u0000a        else if (
  4      531  (
  4      533  ))\u0000d\u0000a                
  4      564  (
  4      568  );\u0000d\u0000a        else\u0000d\u0000a                return 0;\u0000d\u0000a}\u0000d\u0000a
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
----  ---
  79   17
 441   17
 459   17
 849   17
 929   17
 979   17
1199   17
1389   17
1459   17
1699   17
1739   17
2259   17
2379   17
2975   17
3295   17
3309   17
3539   17
3575   17
3865   17
4055   17
5355   17
  79   18
  91   18
 331   18
 441   18
 459   18
 571   18
 849   18
 929   18
 979   18
1199   18
1389   18
1459   18
1699   18
1739   18
2259   18
2379   18
2975   18
3295   18
3309   18
3539   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME             RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  ---------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  2  cscout_defs.h    TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  cscout_incs.h    TRUE     290        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c14-namespace.c  FALSE    616          0     231          0          0     29          69        0       0             4        0          4          0       169      133        1          12           1           0      0      0           2         2      0         0         0
  5  prj2.c           FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
 18     5           5          4      294    3
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 849  getpart         TRUE     TRUE     FALSE     TRUE          4       84      1
 891  foo             FALSE    TRUE     TRUE      FALSE         5       88      1
1389  getfoo          TRUE     TRUE     FALSE     TRUE          4      138      1
1699  get             TRUE     TRUE     FALSE     TRUE          4      169      1
2259  conditional     TRUE     TRUE     FALSE     TRUE          4      225      1
4715  main            FALSE    FALSE    TRUE      FALSE         2      471      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL    IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -------  --------  ------------  ------  ----------
       849     29          0       5          0          0      1          27        0       0             0        0          0          0        12        0      0    2     2        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    1      0      0      0     1       0       2        0       0      1       0       1       1       1   0.0E0  4.75489E0    0.0E0         4           102       4         130
       891    306          0     155          0          0     13          42        0       0             0        0          0          0        61      108      9    1     1        1      0    3      3        0      0         0       0     0       0    0          0      0        3     7     0     6   18      1      0      4     8      16       0        1       7      1       5       4       5       5  25.0E0  66.4386E0  100.0E0         4           309       4         614
      1389     13          0       3          0          0      1          11        0       0             0        0          0          0         5        0      0    1     1        0      0    0      0        0      0         0       0     0       0    0          0      0        0     1     0     0    1      1      0      0     1       0       1        0       1      1       0       1       1       1   0.0E0      2.0E0    0.0E0         4           149       4         161
      1699     40          0       7          0          0      1          38        0       0             0        0          0          0        17        0      0    3     3        1      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    1      0      0      0     1       0       1        0       0      1       0       1       1       1   0.0E0  11.6096E0    0.0E0         4           178       4         217
      2259     49          0      11          0          0      1          47        0       0             0        0          0          0        24        0      5    1     1        3      0    1      0        0      0         0       0     0       0    1          0      1        1     0     0     0    0      0      0      0     0       0       1        0      -1      1       0       3       3       3   0.0E0      8.0E0    0.0E0         4           239       4         287
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1       9       0        0       0      1       1       1       1       1   1.0E0      0.0E0    1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       849        0   849
       891        0    79
      1389        0  1389
      1699        0  1699
      2259        0  2259
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
     891     849
     891     891
     891    1389
     891    1699
     891    2259
    5355    5355
Done
