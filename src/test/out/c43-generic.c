Table: Ids
EID	NAME	READONLY	UNDEFMACRO	MACRO	MACROARG	ORDINARY	SUETAG	SUMEMBER	LABEL	TYPEDEF	ENUM	YACC	FUN	CSCOPE	LSCOPE	UNUSED
49	a	0	0	0	0	1	0	0	0	0	0	0	1	0	1	1
91	MAXDIGIT	0	0	1	0	0	0	0	0	0	0	0	0	0	0	1
209	c	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
331	x	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
369	int_t	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
441	main	1	0	0	0	1	0	0	0	0	0	0	1	0	1	0
489	int_ret	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
571	qqq	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
669	dflt_ret	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
889	tag	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0
891	foo	0	0	0	0	1	0	0	0	0	0	0	1	0	1	1
989	k	0	0	0	0	0	0	1	0	0	0	0	0	0	0	1
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
4	4	49
4	20	209
4	36	369
4	48	489
4	66	669
4	88	889
4	98	989
4	122	209
4	144	369
4	151	489
4	172	889
4	194	669
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
4	0	int\u0000a
4	5	(void)\u0000a{\u0000a	char 
4	21	;\u0000a	typedef int 
4	41	;\u0000a	int 
4	55	 = 2;\u0000a	int 
4	74	 = 0;\u0000a	struct 
4	91	 { int 
4	99	; };\u0000a\u0000a	return _Generic(
4	123	,\u0000a	    char: 1,\u0000a	    
4	149	: 
4	158	,\u0000a	    struct 
4	175	: 3,\u0000a	    default: 
4	202	\u0000a	);\u0000a}\u0000a
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
49	17
209	17
369	17
441	17
489	17
669	17
889	17
989	17
2975	17
3295	17
3575	17
3865	17
4055	17
5355	17
49	18
91	18
209	18
331	18
369	18
441	18
489	18
571	18
669	18
889	18
891	18
989	18
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
4	c43-generic.c	0
5	prj2.c	0
Table: Filemetrics
FID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NCOPIES	NINCFILE	NPFUNCTION	NFFUNCTION	NPVAR	NFVAR	NAGGREGATE	NAMEMBER	NENUM	NEMEMBER
2	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	16	0	1	1	NULL	NULL	NULL	NULL	NULL	18	2	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	0	1	0	0	0	0	0	0
2	1	578	367	29	0	3	22	61	47	NULL	1	1	0	5	0	0	5	37	2	0	0	2	0	3	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	2	5	8	1	1	5	7	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
3	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	15	0	1	1	NULL	NULL	NULL	NULL	NULL	0	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	0	0	0	0	0	0	0	0
3	1	295	187	13	0	2	13	54	28	NULL	1	1	0	1	0	0	0	16	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
4	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	35	0	2	1	NULL	NULL	NULL	NULL	NULL	56	7	2	0	4	0	0	NULL	NULL	0	0	0	0	1	0	0	0	0	0	0	1	2	0	NULL	18	1	0	NULL	5	0	NULL	NULL	1	0	0	0	1	1	0	0
4	1	209	0	63	0	0	16	23	42	NULL	2	1	0	0	0	0	0	55	7	2	0	4	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	1	0	0	9	1	0	0	5	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
5	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	8	0	1	1	NULL	NULL	NULL	NULL	NULL	18	3	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	4	0	NULL	4	4	0	NULL	4	0	NULL	NULL	1	0	2	0	0	0	0	0
5	1	121	34	26	2	0	11	21	14	NULL	1	1	0	1	0	0	1	21	3	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	1	5	4	0	1	5	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
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
49	a	0	1	1	0	4	4	0
891	foo	0	1	1	0	5	88	0
4715	main	0	0	1	0	2	471	0
5355	_cscout_dummy1	0	1	1	1	2	535	1
Table: FunctionDefs
FUNCTIONID	FIDBEGIN	FOFFSETBEGIN	FIDEND	FOFFSETEND
49	4	13	4	208
891	5	95	5	117
5355	2	557	2	577
Table: FunctionMetrics
FUNCTIONID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NGNSOC	NMPARAM	NFPARAM	FANIN	FANOUT	CCYCL1	CCYCL2	CCYCL3	CSTRUC	CHAL	IFLOW
49	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	35	0	1	1	NULL	NULL	NULL	NULL	NULL	49	7	2	1	4	0	0	NULL	NULL	0	0	0	0	1	0	0	0	0	0	0	1	0	0	NULL	8	0	0	NULL	4	0	11	NULL	0	0	0	1	1	1	0.0	44.379	0.0
49	1	196	0	61	0	0	14	23	42	NULL	1	1	0	0	0	0	0	49	7	2	1	4	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	0	0	8	0	0	0	4	0	NULL	0	NULL	0	0	1	1	1	0.0	44.379	0.0
891	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	NULL	NULL	NULL	NULL	NULL	1	0	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	17	NULL	0	0	0	1	1	1	0.0	0.0	0.0
891	1	23	15	4	1	0	2	18	0	NULL	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	NULL	0	0	1	1	1	0.0	0.0	0.0
5355	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	0	0	1	NULL	NULL	NULL	NULL	NULL	5	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	1	NULL	1	0	1	NULL	1	0	9	NULL	0	1	1	1	1	1	1.0	0.0	1.0
5355	1	21	0	3	0	0	1	20	5	NULL	0	1	0	0	0	0	0	5	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	1	0	1	0	1	0	NULL	0	NULL	1	1	1	1	1	1.0	0.0	1.0
Table: FunctionId
FUNCTIONID	ORDINAL	EID
49	0	49
891	0	891
4715	0	441
5355	0	5355
Table: Fcalls
SOURCEID	DESTID
5355	5355
Done
