Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
  91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 619  apoint          FALSE     FALSE       FALSE  FALSE     FALSE     TRUE    FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 759  first           FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
1019  x               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1279  y               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1519  z               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1679  s               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1799  last            FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1879  ap              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    FALSE
1999  point           FALSE     FALSE       FALSE  FALSE     FALSE     TRUE    FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2259  x               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2319  u               FALSE     FALSE       FALSE  FALSE     FALSE     FALSE   TRUE      FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2369  p               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    FALSE
2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
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
  4       61   619
  4       75   759
  4      101  1019
  4      127  1279
  4      151  1519
  4      167  1679
  4      179  1799
  4      187  1879
  4      199  1999
  4      225  2259
  4      231  2319
  4      236  2369
  4      244   441
  4      254  1879
  4      257  1019
  4      273   619
  4      285  1019
  4      294  1279
  4      322   619
  4      334  1019
  4      353   619
  4      365  1279
  4      384   619
  4      396  1519
  4      415   619
  4      427  1279
  4      456   619
  4      468  1519
  4      486  2369
  4      488  2319
  4      490  2259
  4      506  1999
  4      519  2259
  4      541  1999
  4      552  2319
  4      559  2259
  5        8    91
  5       32   331
  5       43   441
  5       56   571
  5       88   891
Table: Rest
FID  FOFFSET  CODE
---  -------  --------------------------------------------------------------------
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
  4       53  \u0000astruct 
  4       67   {\u0000a	int 
  4       80  ;\u0000a	struct {\u0000a		double 
  4      102  ;\u0000a	};\u0000a	struct {\u0000a		double 
  4      128  ;\u0000a		struct {\u0000a			double 
  4      152  ;\u0000a		};\u0000a		char *
  4      168  ;\u0000a	};\u0000a	int 
  4      183  ;\u0000a} 
  4      189  ;\u0000a\u0000astruct 
  4      204   {\u0000a	union {\u0000a		double 
  4      226  ;\u0000a	} 
  4      232  ;\u0000a} 
  4      237  ;\u0000a\u0000aint 
  4      248  () {\u0000a	
  4      256  .
  4      258   = 3;\u0000a	(struct 
  4      279  ) {  .
  4      286   = 42, .
  4      295   = 12, 9, 
  4      309   };\u0000a	(struct 
  4      328  ) {  .
  4      335   = 42 };\u0000a	(struct 
  4      359  ) {  .
  4      366   = 42 };\u0000a	(struct 
  4      390  ) {  .
  4      397   = 42 };\u0000a	(struct 
  4      421  ) {  .
  4      428   = 42, 12, 
  4      443   };\u0000a	(struct 
  4      462  ) {  .
  4      469   = 42, 
  4      480   };\u0000a\u0000a	
  4      487  .
  4      489  .
  4      491   = 3;\u0000a	(struct 
  4      511  ) {  { .
  4      520   = 42 }  };\u0000a	(struct 
  4      546  ) {  .
  4      553   = { .
  4      560   = 42 }  };\u0000a}\u0000a
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
 619   17
 759   17
1019   17
1279   17
1519   17
1679   17
1799   17
1879   17
1999   17
2259   17
2319   17
2369   17
2975   17
3295   17
3575   17
3865   17
4055   17
5355   17
  91   18
 331   18
 441   18
 571   18
 619   18
 759   18
 891   18
1019   18
1279   18
1519   18
1679   18
1799   18
1879   18
1999   18
2259   18
2319   18
2369   18
2975   18
3295   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME                       RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  -------------------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  2  host-defs.h                TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h                TRUE     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c41-unnamed-struct-init.c  FALSE    574         49     164          0          1     37          48        3       0             0        0          0          0       181      182        1          10           1           0      2      0           6         8      0         0         0
  5  prj2.c                     FALSE    121         34      26          2          0     11          21        0       0             1        0          0          1        21       18        1           0           1           0      2      0           0         0      0         0         0
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
2449  main            FALSE    TRUE     TRUE      FALSE         4      244      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -----  --------  ------------  ------  ----------
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      18       0        0       0      0       0       1       1       1   0.0E0      0.0E0  0.0E0         5            95       5         117
      2449    322          0      99          0          0     13          48        3       0             0        0          0          0       125      125     10   25     2       13      0    0      0        0      0         0       0     0       0    0          0      0        0     2     0     0    2      2      0      0     2      13       0        0       0      0       0       1       1       1   0.0E0  163.499E0  0.0E0         4           252       4         573
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      10       0        0       0      1       1       1       1       1   1.0E0      0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       891        0   891
      2449        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
    5355    5355
Done
