Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
  91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
1449  T0              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1539  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1849  T1              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1949  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2239  T2              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2339  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2709  T3              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2809  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3119  T4              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
3219  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3609  T5              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
3709  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
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
  4      120   441
  4      144  1449
  4      150  1449
  4      153  1539
  4      184  1849
  4      191  1849
  4      194  1949
  4      223  2239
  4      230  2239
  4      233  2339
  4      270  2709
  4      277  2709
  4      280  2809
  4      311  3119
  4      318  3119
  4      321  3219
  4      360  3609
  4      367  3609
  4      370  3709
  5        8    91
  5       32   331
  5       43   441
  5       56   571
  5       88   891
Table: Rest
FID  FOFFSET  CODE
---  -------  --------------------------------------------------------------------------------------
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
  4      116  \u0000d\u0000a\u0000d\u0000a
  4      124  ()\u0000d\u0000a{\u0000d\u0000a	typedef int 
  4      146  ;\u0000d\u0000a	
  4      152   
  4      154  ;\u0000d\u0000a\u0000d\u0000a	if (0) {\u0000d\u0000a		typedef int 
  4      186  ;\u0000d\u0000a		
  4      193   
  4      195  ;\u0000d\u0000a	} else {\u0000d\u0000a		typedef int 
  4      225  ;\u0000d\u0000a		
  4      232   
  4      234  ;\u0000d\u0000a	}\u0000d\u0000a\u0000d\u0000a	for (;;) {\u0000d\u0000a		typedef int 
  4      272  ;\u0000d\u0000a		
  4      279   
  4      281  ;\u0000d\u0000a	}\u0000d\u0000a\u0000d\u0000a	do {\u0000d\u0000a		typedef int 
  4      313  ;\u0000d\u0000a		
  4      320   
  4      322  ;\u0000d\u0000a	} while (0);\u0000d\u0000a\u0000d\u0000a	{\u0000d\u0000a		typedef int 
  4      362  ;\u0000d\u0000a		
  4      369   
  4      371  ;\u0000d\u0000a	}\u0000d\u0000a}\u0000d\u0000a
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
 441   17
1449   17
1539   17
1849   17
1949   17
2239   17
2339   17
2709   17
2809   17
2975   17
3119   17
3219   17
3295   17
3575   17
3609   17
3709   17
3865   17
4055   17
5355   17
  91   18
 331   18
 441   18
 571   18
 891   18
1449   18
1539   18
1849   18
1949   18
2239   18
2339   18
2709   18
2809   18
2975   18
3119   18
3219   18
3295   18
3575   18
3609   18
3709   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME           RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  -------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  2  host-defs.h    TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h    TRUE     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c37-yyvalid.c  FALSE    381        112     118          0          1     32          61        0       0             0        0          0          0        73       74        1           8           1           0      0      0           0         0      0         0         0
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
 17     4           4          2
 18     4           4          2
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
 17     4           4          2      471    4
 18     4           4          2      471    4
 18     5           5          2      471    4
Table: Functions
  ID  NAME            ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  --------------  -------  -------  --------  ----------  ---  -------  -----
 891  foo             FALSE    TRUE     TRUE      FALSE         5       88      0
1209  main            FALSE    TRUE     TRUE      FALSE         4      120      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -----  --------  ------------  ------  ----------
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      16       0        0       0      0       0       1       1       1   0.0E0      0.0E0  0.0E0         5            95       5         117
      1209    251          0     111          0          0     26          18        0       0             0        0          0          0        69       69     17    0     0        2      0    1      1        0      0         0       0     1       0    1          0      0        0     0     0     0   18      0      0      0    12       9       0        1       0      0       0       4       4       4   0.0E0  76.1471E0  0.0E0         4           129       4         379
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1       8       0        0       0      1       1       1       1       1   1.0E0      0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       891        0   891
      1209        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
    5355    5355
Done
