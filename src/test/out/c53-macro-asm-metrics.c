Table: Ids
EID	NAME	READONLY	UNDEFMACRO	MACRO	MACROARG	CPPCONST	CPPSTRVAL	ORDINARY	SUETAG	SUMEMBER	LABEL	TYPEDEF	ENUM	YACC	FUN	CSCOPE	LSCOPE	UNUSED
91	MAXDIGIT	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
99	example_function	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
269	input	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0
331	x	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
339	label	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0
441	main	1	0	0	0	0	0	1	0	0	0	0	0	0	1	0	1	0
529	output	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
571	qqq	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
891	foo	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	1	1
2975	__DATE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
3169	printf	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	1	0
3295	__TIME__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
3575	__FILE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
3865	__LINE__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
4055	__STDC__	1	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
5355	_cscout_dummy1	1	0	0	0	0	0	1	0	0	0	0	0	0	1	1	0	0
5489	l1	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1
6449	l2	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1
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
4	9	99
4	26	269
4	33	339
4	52	529
4	181	529
4	214	269
4	275	339
4	316	3169
4	357	529
4	385	339
4	398	529
4	416	3169
4	456	529
4	474	441
4	487	3169
4	528	99
4	548	5489
4	581	3169
4	624	99
4	644	6449
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
4	0	#define  
4	25	(
4	31	, 
4	38	) { \\u0000a    int 
4	58	 = 0; \\u0000a \\u0000a    __asm__ goto ( \\u0000a        
4	114	         \\u0000a        
4	150	        \\u0000a        : [out] 
4	180	(
4	187	)     \\u0000a        : [in] 
4	213	(
4	219	)        \\u0000a        :                        \\u0000a        : 
4	280	                  \\u0000a    ); \\u0000a \\u0000a    
4	322	(
4	355	, 
4	363	); \\u0000a    return; \\u0000a \\u0000a
4	390	: \\u0000a    
4	404	 = 42;\\u0000a    
4	422	(
4	454	, 
4	462	); \\u0000a}\u0000a\u0000aint 
4	478	() {\u0000a    
4	493	(
4	521	);\u0000a    
4	544	(0, 
4	550	);        
4	577	    
4	587	(
4	617	);\u0000a    
4	640	(1, 
4	646	);        
4	665	    return 0;\u0000a}\u0000a
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
18	unspecified
19	Prj1
20	Prj2
Table: IdProj
EID	PID
99	19
269	19
339	19
441	19
529	19
2975	19
3169	19
3295	19
3575	19
3865	19
4055	19
5355	19
5489	19
6449	19
91	20
99	20
269	20
331	20
339	20
441	20
529	20
571	20
891	20
2975	20
3169	20
3295	20
3575	20
3865	20
4055	20
5355	20
5489	20
6449	20
Table: Files
FID	NAME	RO
2	host-defs.h	1
3	host-incs.h	1
4	c53-macro-asm-metrics.c	0
5	prj2.c	0
Table: Filemetrics
FID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	BRACENEST	BRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NASM	NTYPEOF	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NMACROEXPANDTOKEN	NCOPIES	NINCFILE	NPFUNCTION	NFFUNCTION	NPVAR	NFVAR	NAGGREGATE	NAMEMBER	NENUM	NEMEMBER
2	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	16	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	18	2	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	NULL	0	1	0	0	0	0	0	0
2	1	548	367	29	0	3	22	61	47	NULL	1	1	0	0	0	5	0	0	5	37	2	0	0	2	0	3	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	2	5	8	1	1	5	7	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
3	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	15	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	NULL	0	0	0	0	0	0	0	0
3	1	243	187	13	0	2	13	48	28	NULL	1	1	0	0	0	1	0	0	0	16	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
4	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	42	0	2	2	NULL	NULL	NULL	NULL	NULL	NULL	NULL	132	17	4	0	7	0	14	NULL	NULL	0	0	0	0	0	0	0	0	0	0	2	3	2	0	14	0	NULL	34	2	0	NULL	3	6	NULL	NULL	NULL	1	0	0	0	0	0	0	0
4	1	669	20	243	2	0	27	57	41	NULL	1	2	0	0	0	1	0	1	0	96	11	2	0	5	0	8	0	0	0	0	0	0	0	0	0	0	0	0	1	2	1	0	5	0	3	13	2	0	1	4	1	340	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
5	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	8	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	18	3	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	NULL	4	4	0	NULL	4	0	NULL	NULL	NULL	1	0	2	0	0	0	0	0
5	1	106	34	26	2	0	11	21	14	NULL	1	1	0	0	0	1	0	0	1	21	3	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	1	5	4	0	1	5	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
Table: FileProj
FID	PID
2	19
3	19
4	19
1	20
2	20
3	20
4	20
5	20
Table: Definers
PID	CUID	BASEFILEID	DEFINERID
19	4	4	2
20	4	4	2
20	5	5	2
Table: Includers
PID	CUID	BASEFILEID	INCLUDERID
19	2	2	1
19	4	3	1
19	4	4	1
20	2	2	1
20	2	2	1
20	4	3	1
20	4	4	1
20	5	3	1
20	5	5	1
Table: Providers
PID	CUID	PROVIDERID
19	2	2
19	4	4
20	2	2
20	2	2
20	4	4
20	5	5
Table: IncTriggers
PID	CUID	BASEFILEID	DEFINERID	FOFFSET	LEN
19	4	4	2	471	4
20	4	4	2	471	4
20	5	5	2	471	4
Table: Functions
ID	NAME	ISMACRO	DEFINED	DECLARED	FILESCOPED	FID	FOFFSET	FANIN
99	example_function	1	1	0	1	4	9	1
891	foo	0	1	1	0	5	88	0
4749	main	0	1	1	0	4	474	0
4879	printf	0	0	1	0	4	487	2
5355	_cscout_dummy1	0	1	1	1	2	535	1
Table: FunctionDefs
FUNCTIONID	FIDBEGIN	FOFFSETBEGIN	FIDEND	FOFFSETEND
99	4	39	4	469
891	5	95	5	117
4749	4	482	4	680
5355	2	557	2	577
Table: FunctionMetrics
FUNCTIONID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	BRACENEST	BRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NASM	NTYPEOF	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NMACROEXPANDTOKEN	NGNSOC	NMPARAM	NFPARAM	NEPARAM	FANIN	FANOUT	CCYCL1	CCYCL2	CCYCL3	CSTRUC	CHAL	IFLOW
99	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	0	NULL	0	NULL	1	1	1	1	1	1.0	0.0	1.0
99	1	421	0	194	0	0	20	57	41	NULL	1	2	0	0	0	0	0	0	0	55	6	2	1	2	0	6	0	0	0	0	0	0	0	0	0	0	0	0	1	1	1	0	2	0	0	7	1	0	0	2	-1	0	NULL	2	NULL	2	1	1	1	1	1	1.0	25.5412	1.0
891	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	1	0	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	17	NULL	0	NULL	0	0	1	1	1	0.0	0.0	0.0
891	1	23	15	4	1	0	2	18	0	NULL	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	NULL	0	0	0	1	1	1	0.0	0.0	0.0
4749	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	42	0	1	2	NULL	NULL	NULL	NULL	NULL	NULL	NULL	126	17	4	1	7	0	14	NULL	NULL	0	0	0	0	0	0	0	0	0	0	2	3	2	0	6	0	NULL	16	1	0	NULL	2	2	NULL	10	NULL	0	NULL	0	2	1	1	1	0.0	89.6921	0.0
4749	1	200	20	44	2	0	7	54	10	NULL	0	1	0	0	0	0	0	0	0	28	5	0	0	3	0	2	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	2	0	2	4	1	0	1	2	2	170	NULL	0	NULL	0	0	2	1	1	1	0.0	16.2535	0.0
5355	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	0	0	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	NULL	1	0	1	NULL	1	0	NULL	9	NULL	0	NULL	1	1	1	1	1	1.0	0.0	1.0
5355	1	22	0	3	0	0	1	21	5	NULL	0	1	0	0	0	0	0	0	0	5	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	1	0	1	0	1	0	0	NULL	0	NULL	0	1	1	1	1	1	1.0	0.0	1.0
Table: FunctionId
FUNCTIONID	ORDINAL	EID
99	0	99
891	0	891
4749	0	441
4879	0	3169
5355	0	5355
Table: Fcalls
SOURCEID	DESTID
99	4879
4749	99
4749	4879
5355	5355
Done
