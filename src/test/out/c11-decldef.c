Table: Ids
 EID  NAME            READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
----  --------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
  91  MAXDIGIT        FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 269  errno           FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    FALSE
 331  x               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 441  main            TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 449  EFOO            FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 571  qqq             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 689  argc            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 809  argv            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 891  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
1059  argc            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1179  argv            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1309  fputs           FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
1819  xatof           FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
1939  s               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2129  b               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
2429  foo             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  TRUE     FALSE  FALSE  FALSE  TRUE    FALSE   FALSE
2539  fubar           FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
2829  q               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2975  __DATE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3169  afoo            FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  TRUE     FALSE  FALSE  FALSE  TRUE    FALSE   FALSE
3295  __TIME__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3309  afubar          FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
3575  __FILE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3689  HANDLE          FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  TRUE     FALSE  FALSE  FALSE  TRUE    FALSE   FALSE
3865  __LINE__        TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3909  a               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
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
  4       26   269
  4       44   449
  4       59   441
  4       68   689
  4       80   809
  4       96   441
  4      105  1059
  4      117  1179
  4      130  1309
  4      145   269
  4      153   449
  4      181  1819
  4      193  1939
  4      212  2129
  4      242  2429
  4      253  2539
  4      259  2429
  4      282  2829
  4      286  1819
  4      316  3169
  4      330  3309
  4      337  3169
  4      368  3689
  4      377  3689
  4      390  3909
  5        8    91
  5       32   331
  5       43   441
  5       56   571
  5       88   891
Table: Rest
FID  FOFFSET  CODE
---  -------  ------------------------------------------------------------------------
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
  4        0  #include <stdio.h>\u0000d\u0000a\u0000d\u0000aint 
  4       31  ;\u0000d\u0000a\u0000d\u0000a#define 
  4       48   99\u0000d\u0000a\u0000d\u0000aint 
  4       63  (int 
  4       72  , char *
  4       84  []);\u0000d\u0000a\u0000d\u0000aint 
  4      100  (int 
  4      109  , char *
  4      121  [])\u0000d\u0000a{\u0000d\u0000a	
  4      135  (
  4      140  );\u0000d\u0000a	
  4      150   = 
  4      157  ;\u0000d\u0000a\u0000d\u0000a}\u0000d\u0000a\u0000d\u0000aextern double 
  4      186  (char *
  4      194  );\u0000d\u0000a\u0000d\u0000aint * const 
  4      213  ;\u0000d\u0000a\u0000d\u0000atypedef unsigned short *
  4      245  ;\u0000d\u0000avoid 
  4      258  (
  4      262   const*) {\u0000d\u0000a	double 
  4      283   = 
  4      291  (
  4      295  );\u0000d\u0000a}\u0000d\u0000a\u0000d\u0000atypedef int 
  4      320  [];\u0000d\u0000avoid 
  4      336  (
  4      341   const*);\u0000d\u0000a\u0000d\u0000atypedef void *
  4      374  ;\u0000d\u0000a
  4      383   const 
  4      391   = 3;\u0000d\u0000a\u0000d\u0000a
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
 269   17
 441   17
 449   17
 689   17
 809   17
1059   17
1179   17
1309   17
1819   17
1939   17
2129   17
2429   17
2539   17
2829   17
2975   17
3169   17
3295   17
3309   17
3575   17
3689   17
3865   17
3909   17
4055   17
5355   17
  91   18
 269   18
 331   18
 441   18
 449   18
 571   18
 689   18
 809   18
 891   18
1059   18
1179   18
1309   18
1819   18
1939   18
2129   18
2429   18
2539   18
2829   18
2975   18
3169   18
3295   18
3309   18
3575   18
3689   18
3865   18
3909   18
4055   18
5355   18
Table: Files
FID  NAME           RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  -------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  2  host-defs.h    TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h    TRUE     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c11-decldef.c  FALSE    400          0     102          0          0     30          34        2       0             2        0          0          1       108      102        1           2           2           0      3      0           0         0      0         0         1
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
 969  main            FALSE    TRUE     TRUE      FALSE         4       96      0
1309  fputs           FALSE    FALSE    TRUE      FALSE         4      130      1
1819  xatof           FALSE    FALSE    TRUE      FALSE         4      181      1
2539  fubar           FALSE    TRUE     TRUE      FALSE         4      253      0
3309  afubar          FALSE    FALSE    TRUE      FALSE         4      330      0
5355  _cscout_dummy1  FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -----  --------  ------------  ------  ----------
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      23       0        0       0      0       0       1       1       1   0.0E0      0.0E0  0.0E0         5            95       5         117
       969     37          0      13          0          0      4          15        1       0             0        0          0          0        10       10      2    1     1        0      0    0      0        0      0         0       0     0       0    0          0      0        0     2     0     1    3      2      0      1     3      12       0        0       0      0       1       1       1       1   0.0E0      8.0E0  0.0E0         4           127       4         163
      2539     29          0       9          0          0      2          24        1       0             0        0          0          0         9        9      1    1     1        0      0    0      0        0      0         0       0     0       0    0          0      0        0     1     0     0    2      1      0      0     2      21       1        0       0      0       1       1       1       1   0.0E0  4.75489E0  0.0E0         4           272       4         300
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      15       0        0       0      1       1       1       1       1   1.0E0      0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
       891        0   891
       969        0   441
      1309        0  1309
      1819        0  1819
      2539        0  2539
      3309        0  3309
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
     969    1309
    2539    1819
    5355    5355
Done
