Table: Ids
EID	NAME	READONLY	UNDEFMACRO	MACRO	MACROARG	ORDINARY	SUETAG	SUMEMBER	LABEL	TYPEDEF	ENUM	YACC	FUN	CSCOPE	LSCOPE	UNUSED
59	e	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1
91	MAXDIGIT	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1
119	A	0	0	0	0	1	0	0	0	0	1	0	0	1	0	0
169	B	0	0	0	0	1	0	0	0	0	1	0	0	1	0	0
219	C	0	0	0	0	1	0	0	0	0	1	0	0	1	0	0
319	D	0	0	0	0	1	0	0	0	0	1	0	0	1	0	0
331	x	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
369	E	0	0	0	0	1	0	0	0	0	1	0	0	1	0	0
441	main	1	0	0	0	1	0	0	0	0	0	0	1	0	1	0
499	a	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
571	qqq	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
891	foo	0	0	0	0	1	0	0	0	0	0	0	1	0	1	1
1329	e2	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1
1399	A2	0	0	0	0	1	0	0	0	0	1	0	0	1	0	1
1499	B2	0	0	0	0	1	0	0	0	0	1	0	0	1	0	1
1559	C2	0	0	0	0	1	0	0	0	0	1	0	0	1	0	1
1669	D2	0	0	0	0	1	0	0	0	0	1	0	0	1	0	1
1729	E2	0	0	0	0	1	0	0	0	0	1	0	0	1	0	1
2975	__DATE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	1
3295	__TIME__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	1
3575	__FILE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	1
3865	__LINE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	1
4055	__STDC__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	1
5355	_cscout_dummy1	1	0	0	0	1	0	0	0	0	0	0	1	1	0	0
Table: Tokens
FID	FOFFSET	EID
2	297	2975
2	329	3295
2	357	3575
2	386	3865
2	405	4055
2	471	441
2	535	5355
2	558	5355
4	5	59
4	11	119
4	16	169
4	21	219
4	31	319
4	36	369
4	49	499
4	60	119
4	71	169
4	75	219
4	86	219
4	101	319
4	112	369
4	132	1329
4	139	1399
4	149	1499
4	155	1559
4	166	1669
4	172	1729
5	8	91
5	32	331
5	43	441
5	56	571
5	88	891
Table: Rest
FID	FOFFSET	CODE
2	287	\u0000a\u0000a#define 
2	305	  
2	320	\u0000a#define 
2	337	 
2	348	\u0000a#define 
2	365	 
2	377	\u0000a#define 
2	394	 1\u0000a#define 
2	413	 1\u0000a\u0000a
2	466	\u0000aint 
2	475	();\u0000a
2	522	\u0000astatic void 
2	549	(void) { 
2	572	(); }\u0000a
3	152	\u0000a\u0000a\u0000a#pragma includepath 
3	195	\u0000a
3	239	\u0000astatic void _cscout_dummy2(void) { _cscout_dummy2(); }\u0000a
4	0	enum 
4	6	 {\u0000d\u0000a	
4	12	,\u0000d\u0000a	
4	17	,\u0000d\u0000a	
4	22	 = 18,\u0000d\u0000a	
4	32	,\u0000d\u0000a	
4	37	\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000aint 
4	50	[] = {\u0000d\u0000a	[
4	61	] = 4,\u0000d\u0000a	[
4	72	 + 
4	76	] = 4,\u0000d\u0000a	[
4	87	 * 2] = 4,\u0000d\u0000a	[
4	102	] = 4,\u0000d\u0000a	[
4	113	] = 4,\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000aenum 
4	134	 {\u0000d\u0000a	
4	141	 = 2,\u0000d\u0000a	
4	151	,\u0000d\u0000a	
4	157	 = 18,\u0000d\u0000a	
4	168	,\u0000d\u0000a	
4	174	,\u0000d\u0000a};\u0000d\u0000a\u0000d\u0000a
5	0	#define 
5	16	 11\u0000d\u0000aextern int 
5	33	;\u0000d\u0000aextern 
5	47	();\u0000d\u0000aint 
5	59	;\u0000d\u0000a\u0000d\u0000a
5	86	\u0000d\u0000a
5	91	() {\u0000d\u0000a	
5	116	}\u0000d\u0000a\u0000d\u0000a
Table: Projects
PID	NAME
16	unspecified
17	Prj1
18	Prj2
Table: IdProj
EID	PID
59	17
119	17
169	17
219	17
319	17
369	17
441	17
499	17
1329	17
1399	17
1499	17
1559	17
1669	17
1729	17
2975	17
3295	17
3575	17
3865	17
4055	17
5355	17
59	18
91	18
119	18
169	18
219	18
319	18
331	18
369	18
441	18
499	18
571	18
891	18
1329	18
1399	18
1499	18
1559	18
1669	18
1729	18
2975	18
3295	18
3575	18
3865	18
4055	18
5355	18
Table: Files
FID	NAME	RO
2	host-defs.h	1
3	host-incs.h	1
4	c35-enum-designator.c	0
5	prj2.c	0
Table: Filemetrics
FID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NCOPIES	NINCFILE	NPFUNCTION	NFFUNCTION	NPVAR	NFVAR	NAGGREGATE	NAMEMBER	NENUM	NEMEMBER
2	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	16	0	1	1	NULL	NULL	NULL	NULL	NULL	18	2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	0	1	0	0	0	0	0	0
2	1	578	367	29	0	3	22	61	47	NULL	1	1	0	5	0	0	5	37	2	0	0	2	0	3	0	0	0	0	0	0	0	0	0	0	0	0	1	2	5	8	1	1	5	7	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
3	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	15	0	1	1	NULL	NULL	NULL	NULL	NULL	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	0	0	0	0	0	0	0	0
3	1	295	187	13	0	2	13	54	28	NULL	1	1	0	1	0	0	0	16	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
4	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	73	0	1	0	NULL	NULL	NULL	NULL	NULL	78	3	11	0	9	0	0	0	0	0	0	0	0	0	0	0	0	0	0	2	32	NULL	34	1	10	NULL	11	0	NULL	NULL	0	0	1	0	0	0	2	10
4	1	183	0	90	0	0	24	14	80	NULL	1	0	0	0	0	0	0	77	3	11	0	9	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	16	0	17	1	10	0	11	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
5	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	8	0	1	1	NULL	NULL	NULL	NULL	NULL	18	3	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	NULL	4	4	0	NULL	4	0	NULL	NULL	1	0	2	0	0	0	0	0
5	1	121	34	26	2	0	11	21	14	NULL	1	1	0	1	0	0	1	21	3	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	1	5	4	0	1	5	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
Table: FileProj
FID	PID
2	17
3	17
4	17
1	18
2	18
3	18
4	18
5	18
Table: Definers
PID	CUID	BASEFILEID	DEFINERID
18	5	5	2
Table: Includers
PID	CUID	BASEFILEID	INCLUDERID
17	2	2	1
17	4	3	1
17	4	4	1
18	2	2	1
18	2	2	1
18	4	3	1
18	4	4	1
18	5	3	1
18	5	5	1
Table: Providers
PID	CUID	PROVIDERID
17	2	2
17	4	4
18	2	2
18	2	2
18	4	4
18	5	5
Table: IncTriggers
PID	CUID	BASEFILEID	DEFINERID	FOFFSET	LEN
18	5	5	2	471	4
Table: Functions
ID	NAME	ISMACRO	DEFINED	DECLARED	FILESCOPED	FID	FOFFSET	FANIN
891	foo	0	1	1	0	5	88	0
4715	main	0	0	1	0	2	471	0
5355	_cscout_dummy1	0	1	1	1	2	535	1
Table: FunctionDefs
FUNCTIONID	FIDBEGIN	FOFFSETBEGIN	FIDEND	FOFFSETEND
-3126566792108663513	2	557	2	577
-1875940075246998904	5	95	5	117
Table: FunctionMetrics
FUNCTIONID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NGNSOC	NMPARAM	NFPARAM	FANIN	FANOUT	CCYCL1	CCYCL2	CCYCL3	CSTRUC	CHAL	IFLOW
891	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	NULL	NULL	NULL	NULL	NULL	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	17	NULL	0	0	0	1	1	1	0.0	0.0	0.0
891	1	23	15	4	1	0	2	18	0	NULL	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	NULL	0	0	1	1	1	0.0	0.0	0.0
5355	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	0	0	1	NULL	NULL	NULL	NULL	NULL	5	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	NULL	1	0	1	NULL	1	0	9	NULL	0	1	1	1	1	1	1.0	0.0	1.0
5355	1	21	0	3	0	0	1	20	5	NULL	0	1	0	0	0	0	0	5	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	1	0	1	0	1	0	NULL	0	NULL	1	1	1	1	1	1.0	0.0	1.0
Table: FunctionId
FUNCTIONID	ORDINAL	EID
891	0	891
4715	0	441
5355	0	5355
Table: Fcalls
SOURCEID	DESTID
5355	5355
Done
