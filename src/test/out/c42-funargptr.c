Table: Ids
 EID  NAME             READONLY  UNDEFMACRO  MACRO  MACROARG  ORDINARY  SUETAG  SUMEMBER  LABEL  TYPEDEF  ENUM   YACC   FUN    CSCOPE  LSCOPE  UNUSED
----  ---------------  --------  ----------  -----  --------  --------  ------  --------  -----  -------  -----  -----  -----  ------  ------  ------
  79  simple_function  FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
  91  MAXDIGIT         FALSE     FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 309  d                FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 331  x                FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 441  main             TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    FALSE
 571  qqq              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
 619  f                FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
 679  fa               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
 789  fx               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 869  fxa              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
 891  foo              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
 969  fb               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
1089  rfa              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
1209  lb               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2079  f2               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
2149  f2a              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2279  rf2a             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2409  rf2b             FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
2779  f4a              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   TRUE    TRUE
2879  f3               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   TRUE    TRUE
2949  f3a              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
2975  __DATE__         TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3069  f4               FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   FALSE   FALSE   FALSE
3139  f4a              FALSE     FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   FALSE
3295  __TIME__         TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3575  __FILE__         TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
3865  __LINE__         TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
4055  __STDC__         TRUE      FALSE       TRUE   FALSE     FALSE     FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  FALSE  FALSE   FALSE   TRUE
5355  _cscout_dummy1   TRUE      FALSE       FALSE  FALSE     TRUE      FALSE   FALSE     FALSE  FALSE    FALSE  FALSE  TRUE   TRUE    FALSE   FALSE
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
  4       30   309
  4       42   309
  4       61   619
  4       67   679
  4       78   789
  4       86   869
  4       96   969
  4      108  1089
  4      120  1209
  4      126  1209
  4      139   679
  4      145   969
  4      158    79
  4      207  2079
  4      214  2149
  4      227  2279
  4      240  2409
  4      249  2149
  4      277  2779
  4      287  2879
  4      294  2949
  4      306  3069
  4      313  3139
  4      330  3139
  4      350  3069
  4      353  2949
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
  4        0  double 
  4       22  (double 
  4       31  ) { return 
  4       43   + 1; }\u0000a\u0000adouble (*
  4       62  (int 
  4       69  , void (*
  4       80  )(int 
  4       89  ), int 
  4       98  ))(double 
  4      111  )\u0000a{\u0000a	int 
  4      122  ;\u0000a\u0000a	
  4      128   = 3;\u0000a	if (
  4      141   && 
  4      147  )\u0000a		return 
  4      173  ;\u0000a	else\u0000a		return 0;\u0000a}\u0000a\u0000adouble (*(*
  4      209  (int 
  4      217  ))(double 
  4      231  ))(float 
  4      244  )\u0000a{\u0000a	
  4      252  --;\u0000a	return 0;\u0000a}\u0000a\u0000adouble 
  4      280  ;\u0000a\u0000aint 
  4      289  (int 
  4      297  )\u0000a{\u0000a	int 
  4      308  (int 
  4      316  )\u0000a	{\u0000a		return 
  4      333   + 2;\u0000a	}\u0000a	return 
  4      352  (
  4      356  );\u0000a}\u0000a
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
 309   17
 441   17
 619   17
 679   17
 789   17
 869   17
 969   17
1089   17
1209   17
2079   17
2149   17
2279   17
2409   17
2779   17
2879   17
2949   17
2975   17
3069   17
3139   17
3295   17
3575   17
3865   17
4055   17
5355   17
  79   18
  91   18
 309   18
 331   18
 441   18
 571   18
 619   18
 679   18
 789   18
 869   18
 891   18
 969   18
1089   18
1209   18
2079   18
2149   18
2279   18
2409   18
2779   18
2879   18
2949   18
2975   18
3069   18
3139   18
3295   18
3575   18
3865   18
4055   18
5355   18
Table: Files
FID  NAME             RO     NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NCOPIES  NSTATEMENT  NPFUNCTION  NFFUNCTION  NPVAR  NFVAR  NAGGREGATE  NAMEMBER  NENUM  NEMEMBER  NINCFILE
---  ---------------  -----  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -------  ----------  ----------  ----------  -----  -----  ----------  --------  -----  --------  --------
  2  host-defs.h      TRUE     578        367      29          0          3     22          61        3       0             5        0          0          5        37       18        1           1           0           1      0      0           0         0      0         0         0
  3  host-incs.h      TRUE     295        187      13          0          2     13          54        1       0             1        0          0          0        16        0        1           0           0           0      0      0           0         0      0         0         0
  4  c42-funargptr.c  FALSE    361          0      83          0          0     29          60        0       0             0        0          0          0       119      120        1          10           4           0      1      0           0         0      0         0         0
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
  ID  NAME             ISMACRO  DEFINED  DECLARED  FILESCOPED  FID  FOFFSET  FANIN
----  ---------------  -------  -------  --------  ----------  ---  -------  -----
  79  simple_function  FALSE    TRUE     TRUE      FALSE         4        7      1
 619  f                FALSE    TRUE     TRUE      FALSE         4       61      0
 891  foo              FALSE    TRUE     TRUE      FALSE         5       88      0
2079  f2               FALSE    TRUE     TRUE      FALSE         4      207      0
2879  f3               FALSE    TRUE     TRUE      FALSE         4      287      0
3069  f4               FALSE    TRUE     TRUE      FALSE         4      306      1
4715  main             FALSE    FALSE    TRUE      FALSE         2      471      0
5355  _cscout_dummy1   FALSE    TRUE     TRUE      TRUE          2      535      1
Table: FunctionMetrics
FUNCTIONID  NCHAR  NCCOMMENT  NSPACE  NLCOMMENT  NBCOMMENT  NLINE  MAXLINELEN  NSTRING  NULINE  NPPDIRECTIVE  NPPCOND  NPPFMACRO  NPPOMACRO  NPPTOKEN  NCTOKEN  NSTMT  NOP  NUOP  NNCONST  NCLIT  NIF  NELSE  NSWITCH  NCASE  NDEFAULT  NBREAK  NFOR  NWHILE  NDO  NCONTINUE  NGOTO  NRETURN  NPID  NFID  NMID  NID  NUPID  NUFID  NUMID  NUID  NGNSOC  NPARAM  MAXNEST  NLABEL  FANIN  FANOUT  CCYCL1  CCYCL2  CCYCL3  CSTRUC       CHAL  IFLOW  FIDBEGIN  FOFFSETBEGIN  FIDEND  FOFFSETEND
----------  -----  ---------  ------  ---------  ---------  -----  ----------  -------  ------  ------------  -------  ---------  ---------  --------  -------  -----  ---  ----  -------  -----  ---  -----  -------  -----  --------  ------  ----  ------  ---  ---------  -----  -------  ----  ----  ----  ---  -----  -----  -----  ----  ------  ------  -------  ------  -----  ------  ------  ------  ------  ------  ---------  -----  --------  ------------  ------  ----------
        79     17          0       6          0          0      1          16        0       0             0        0          0          0         6        6      1    1     1        1      0    0      0        0      0         0       0     0       0    0          0      0        1     0     0     0    1      0      0      0     1      11       1        0       0      1       0       1       1       1   0.0E0  4.75489E0  0.0E0         4            34       4          50
       619     81          0      25          0          0      9          25        0       0             0        0          0          0        21       21      5    2     2        2      0    1      1        0      0         0       0     0       0    0          0      0        2     1     0     0    5      1      0      0     4      13       3        1       0      0       1       2       3       3   0.0E0     27.0E0  0.0E0         4           114       4         194
       891     23         15       4          1          0      2          18        0       0             0        0          0          0         1        1      0    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     0     0    0      0      0      0     0      22       0        0       0      0       0       1       1       1   0.0E0      0.0E0  0.0E0         5            95       5         117
      2079     22          0       7          0          0      4          10        0       0             0        0          0          0         7        7      2    1     1        1      0    0      0        0      0         0       0     0       0    0          0      0        1     0     0     0    1      0      0      0     1      15       1        0       0      0       0       1       1       1   0.0E0  4.75489E0  0.0E0         4           247       4         268
      2879     39          0      10          0          0      4          18        0       0             0        0          0          0        14       14      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        1     0     0     0    4      0      0      0     3      19       1        0       0      0       1       1       1       1   0.0E0  6.33985E0  0.0E0         4           300       4         360
      3069     22          0       9          0          0      3          17        0       0             0        0          0          0         6        6      1    1     1        1      0    0      0        0      0         0       0     0       0    0          0      0        1     0     0     0    1      0      0      0     1      20       1        0       0      1       0       1       1       1   0.0E0  4.75489E0  0.0E0         4           320       4         341
      5355     21          0       3          0          0      1          20        0       0             0        0          0          0         5        5      1    0     0        0      0    0      0        0      0         0       0     0       0    0          0      0        0     0     1     0    1      0      1      0     1      14       0        0       0      1       1       1       1       1   1.0E0      0.0E0  1.0E0         2           557       2         577
Table: FunctionId
FUNCTIONID  ORDINAL   EID
----------  -------  ----
        79        0    79
       619        0   619
       891        0   891
      2079        0  2079
      2879        0  2879
      3069        0  3069
      4715        0   441
      5355        0  5355
Table: Fcalls
SOURCEID  DESTID
--------  ------
     619      79
    2879    3069
    5355    5355
Done
