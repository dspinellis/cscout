Table: Ids
EID	NAME	READONLY	UNDEFMACRO	MACRO	FUNMACRO	MACROARG	CPPCONST	CPPSTRVAL	DEFCCONSTVAL	NOTDEFCCONSTVAL	EXPCCONSTVAL	NOTEXPCCONSTVAL	ORDINARY	SUETAG	SUMEMBER	LABEL	TYPEDEF	ENUM	YACC	FUN	CSCOPE	LSCOPE	UNUSED
73	MAXDIGIT	0	0	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
265	x	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
353	main	1	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	1	0
457	qqq	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	1	1
713	foo	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	0	1	1
2381	__DATE__	1	0	1	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1
2637	__TIME__	1	0	1	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1
2861	__FILE__	1	0	1	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1
3093	__LINE__	1	0	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
3245	__STDC__	1	0	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	1
4285	_cscout_dummy1	1	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	1	1	0	0
Table: Tokens
FID	FOFFSET	EID
2	297	2381
2	329	2637
2	357	2861
2	386	3093
2	405	3245
2	471	353
2	535	4285
2	558	4285
4	8	73
4	32	265
4	43	353
4	56	457
4	88	713
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
4	16	 11\u0000d\u0000aextern int 
4	33	;\u0000d\u0000aextern 
4	47	();\u0000d\u0000aint 
4	59	;\u0000d\u0000a\u0000d\u0000a
4	86	\u0000d\u0000a
4	91	() {\u0000d\u0000a	
4	116	}\u0000d\u0000a\u0000d\u0000a
Table: Projects
PID	NAME
23	unspecified
24	Prj1
25	Prj2
Table: IdProj
EID	PID
73	24
265	24
353	24
457	24
713	24
2381	24
2637	24
2861	24
3093	24
3245	24
4285	24
73	25
265	25
353	25
457	25
713	25
2381	25
2637	25
2861	25
3093	25
3245	25
4285	25
Table: Files
FID	NAME	RO
2	host-defs.h	1
3	host-incs.h	1
4	prj2.c	0
Table: Filemetrics
FID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	BRACENEST	BRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NASM	NTYPEOF	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NMACROEXPANDTOKEN	NCOPIES	NINCFILE	NPFUNCTION	NFFUNCTION	NPVAR	NFVAR	NAGGREGATE	NAMEMBER	NENUM	NEMEMBER
2	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	16	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	18	2	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	NULL	0	1	0	0	0	0	0	0
2	1	548	367	29	0	3	22	61	47	NULL	1	1	0	0	0	5	0	0	5	37	2	0	0	2	0	3	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	2	5	8	1	1	5	7	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
3	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	15	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	NULL	NULL	0	0	0	0	0	0	0	0
3	1	243	187	13	0	2	13	48	28	NULL	1	1	0	0	0	1	0	0	0	16	1	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
4	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	8	0	1	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	18	3	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	12	0	NULL	12	4	0	NULL	4	0	NULL	NULL	NULL	1	0	2	0	0	0	0	0
4	1	106	34	26	2	0	11	21	14	NULL	1	1	0	0	0	1	0	0	1	21	3	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	4	0	1	5	4	0	1	5	0	0	1	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	NULL
Table: FileProj
FID	PID
2	24
3	24
4	24
1	25
2	25
3	25
4	25
Table: Definers
PID	CUID	BASEFILEID	DEFINERID
24	4	4	2
25	4	4	2
25	4	4	2
Table: Includers
PID	CUID	BASEFILEID	INCLUDERID
24	2	2	1
24	4	3	1
24	4	4	1
25	2	2	1
25	2	2	1
25	4	3	1
25	4	3	1
25	4	4	1
25	4	4	1
Table: Providers
PID	CUID	PROVIDERID
24	2	2
24	4	4
25	2	2
25	2	2
25	4	4
25	4	4
Table: IncTriggers
PID	CUID	BASEFILEID	DEFINERID	FOFFSET	LEN
24	4	4	2	471	4
25	4	4	2	471	4
25	4	4	2	471	4
Table: Functions
ID	NAME	ISMACRO	DEFINED	DECLARED	FILESCOPED	FID	FOFFSET	FANIN
713	foo	0	1	1	0	4	88	0
3773	main	0	0	1	0	2	471	0
4285	_cscout_dummy1	0	1	1	1	2	535	1
Table: FunctionDefs
FUNCTIONID	FIDBEGIN	FOFFSETBEGIN	FIDEND	FOFFSETEND
713	4	95	4	117
4285	2	557	2	577
Table: FunctionMetrics
FUNCTIONID	PRECPP	NCHAR	NCCOMMENT	NSPACE	NLCOMMENT	NBCOMMENT	NLINE	MAXLINELEN	MAXSTMTLEN	MAXSTMTNEST	MAXBRACENEST	MAXBRACKNEST	BRACENEST	BRACKNEST	NULINE	NPPDIRECTIVE	NPPCOND	NPPFMACRO	NPPOMACRO	NTOKEN	NSTMT	NOP	NUOP	NNCONST	NCLIT	NSTRING	NPPCONCATOP	NPPSTRINGOP	NIF	NELSE	NSWITCH	NCASE	NDEFAULT	NBREAK	NFOR	NWHILE	NDO	NCONTINUE	NGOTO	NRETURN	NASM	NTYPEOF	NPID	NFID	NMID	NID	NUPID	NUFID	NUMID	NUID	NLABEL	NMACROEXPANDTOKEN	NGNSOC	NMPARAM	NFPARAM	NEPARAM	FANIN	FANOUT	CCYCL1	CCYCL2	CCYCL3	CSTRUC	CHAL	IFLOW
713	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	0	0	0	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	1	0	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	0	0	NULL	0	0	NULL	16	NULL	0	NULL	0	0	1	1	1	0.0	0.0	0.0
713	1	23	15	4	1	0	2	18	0	NULL	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	NULL	0	NULL	0	0	0	1	1	1	0.0	0.0	0.0
4285	0	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	0	0	1	NULL	NULL	NULL	NULL	NULL	NULL	NULL	5	1	0	0	0	0	0	NULL	NULL	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	NULL	1	0	1	NULL	1	0	NULL	11	NULL	0	NULL	1	1	1	1	1	1.0	0.0	1.0
4285	1	22	0	3	0	0	1	21	5	NULL	0	1	0	0	0	0	0	0	0	5	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	1	0	1	0	1	0	0	NULL	0	NULL	0	1	1	1	1	1	1.0	0.0	1.0
Table: FunctionId
FUNCTIONID	ORDINAL	EID
713	0	713
3773	0	353
4285	0	4285
Table: Fcalls
SOURCEID	DESTID
4285	4285
Done
