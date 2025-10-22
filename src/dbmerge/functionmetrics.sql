-- Create global functiondefs table with suitably mapped entries
-- with the maximum of the merged ones.

CREATE TABLE new_FUNCTIONMETRICS(
  FUNCTIONID BIGINT, -- Function identifier key
  PRECPP BOOLEAN, -- True for values before the cpp false for values after it
  NCHAR INTEGER, -- Number of characters
  NCCOMMENT INTEGER, -- Number of comment characters
  NSPACE INTEGER, -- Number of space characters
  NLCOMMENT INTEGER, -- Number of line comments
  NBCOMMENT INTEGER, -- Number of block comments
  NLINE INTEGER, -- Number of lines
  MAXLINELEN INTEGER, -- Maximum number of characters in a line
  MAXSTMTLEN INTEGER, -- Maximum number of tokens in a statement
  MAXSTMTNEST INTEGER, -- Maximum level of statement nesting
  MAXBRACENEST INTEGER, -- Maximum level of brace nesting
  MAXBRACKNEST INTEGER, -- Maximum level of bracket nesting
  BRACENEST INTEGER, -- Dangling brace nesting
  BRACKNEST INTEGER, -- Dangling bracket nesting
  NULINE INTEGER, -- Number of unprocessed lines
  NPPDIRECTIVE INTEGER, -- Number of C preprocessor directives
  NPPCOND INTEGER, -- Number of processed C preprocessor conditionals (ifdef, if, elif)
  NPPFMACRO INTEGER, -- Number of defined C preprocessor function-like macros
  NPPOMACRO INTEGER, -- Number of defined C preprocessor object-like macros
  NTOKEN INTEGER, -- Number of tokens
  NSTMT INTEGER, -- Number of statements or declarations
  NOP INTEGER, -- Number of operators
  NUOP INTEGER, -- Number of unique operators
  NNCONST INTEGER, -- Number of numeric constants
  NCLIT INTEGER, -- Number of character literals
  NSTRING INTEGER, -- Number of character strings
  NPPCONCATOP INTEGER, -- Number of token concatenation operators (##)
  NPPSTRINGOP INTEGER, -- Number of token stringification operators (#)
  NIF INTEGER, -- Number of if statements
  NELSE INTEGER, -- Number of else clauses
  NSWITCH INTEGER, -- Number of switch statements
  NCASE INTEGER, -- Number of case labels
  NDEFAULT INTEGER, -- Number of default labels
  NBREAK INTEGER, -- Number of break statements
  NFOR INTEGER, -- Number of for statements
  NWHILE INTEGER, -- Number of while statements
  NDO INTEGER, -- Number of do statements
  NCONTINUE INTEGER, -- Number of continue statements
  NGOTO INTEGER, -- Number of goto statements
  NRETURN INTEGER, -- Number of return statements
  NASM INTEGER, -- Number of assembly statements
  NTYPEOF INTEGER, -- Number of typeof operators
  NPID INTEGER, -- Number of project-scope identifiers
  NFID INTEGER, -- Number of file-scope (static) identifiers
  NMID INTEGER, -- Number of macro identifiers
  NID INTEGER, -- Total number of object and object-like identifiers
  NUPID INTEGER, -- Number of unique project-scope identifiers
  NUFID INTEGER, -- Number of unique file-scope (static) identifiers
  NUMID INTEGER, -- Number of unique macro identifiers
  NUID INTEGER, -- Number of unique object and object-like identifiers
  NLABEL INTEGER, -- Number of goto labels
  NMACROEXPANDTOKEN INTEGER, -- Tokens added by macro expansion
  NGNSOC INTEGER, -- Number of global namespace occupants at function's top
  NMPARAM INTEGER, -- Number of parameters (for macros)
  NFPARAM INTEGER, -- Number of parameters (for functions)
  NEPARAM INTEGER, -- Number of passed non-expression macro parameters
  FANIN INTEGER, -- Fan-in (number of calling functions)
  FANOUT INTEGER, -- Fan-out (number of called functions)
  CCYCL1 INTEGER, -- Cyclomatic complexity (control statements)
  CCYCL2 INTEGER, -- Extended cyclomatic complexity (includes branching operators)
  CCYCL3 INTEGER, -- Maximum cyclomatic complexity (includes branching operators and all switch branches)
  CSTRUC REAL, -- Structure complexity (Henry and Kafura)
  CHAL REAL, -- Halstead volume
  IFLOW REAL, -- Information flow metric (Henry and Selig)
  PRIMARY KEY(FUNCTIONID, PRECPP),
  FOREIGN KEY(FUNCTIONID) REFERENCES FUNCTIONS(ID)
);


-- Pair with global map
CREATE TABLE fma AS
  SELECT functionid_map.global_id AS gfunctionid, f.*
    FROM adb.functionmetrics AS f
    INNER JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid = 5 AND functionid_map.id = f.functionid;

CREATE TABLE fmb AS
  SELECT functionid_map.global_id AS gfunctionid, f.*
    FROM functionmetrics AS f
    INNER JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid != 5 AND functionid_map.id = f.functionid;

CREATE UNIQUE INDEX idx_fma_gfunctiond_precpp  ON fma(gfunctionid, precpp);
CREATE UNIQUE INDEX idx_fmb_gfunctiond_precpp  ON fmb(gfunctionid, precpp);

-- Merge
INSERT INTO new_functionmetrics
  SELECT
    Coalesce(fma.gfunctionid, fmb.gfunctionid) AS functionid,
    Coalesce(fma.precpp, fmb.precpp) AS precpp,
    Max(Coalesce(fma.nchar, fmb.nchar), Coalesce(fmb.nchar, fma.nchar)) AS nchar,
    Max(Coalesce(fma.nccomment, fmb.nccomment), Coalesce(fmb.nccomment, fma.nccomment)) AS nccomment,
    Max(Coalesce(fma.nspace, fmb.nspace), Coalesce(fmb.nspace, fma.nspace)) AS nspace,
    Max(Coalesce(fma.nlcomment, fmb.nlcomment), Coalesce(fmb.nlcomment, fma.nlcomment)) AS nlcomment,
    Max(Coalesce(fma.nbcomment, fmb.nbcomment), Coalesce(fmb.nbcomment, fma.nbcomment)) AS nbcomment,
    Max(Coalesce(fma.nline, fmb.nline), Coalesce(fmb.nline, fma.nline)) AS nline,
    Max(Coalesce(fma.maxlinelen, fmb.maxlinelen), Coalesce(fmb.maxlinelen, fma.maxlinelen)) AS maxlinelen,
    Max(Coalesce(fma.maxstmtlen, fmb.maxstmtlen), Coalesce(fmb.maxstmtlen, fma.maxstmtlen)) AS maxstmtlen,
    Max(Coalesce(fma.maxstmtnest, fmb.maxstmtnest), Coalesce(fmb.maxstmtnest, fma.maxstmtnest)) AS maxstmtnest,
    Max(Coalesce(fma.maxbracenest, fmb.maxbracenest), Coalesce(fmb.maxbracenest, fma.maxbracenest)) AS maxbracenest,
    Max(Coalesce(fma.maxbracknest, fmb.maxbracknest), Coalesce(fmb.maxbracknest, fma.maxbracknest)) AS maxbracknest,
    Max(Coalesce(fma.bracenest, fmb.bracenest), Coalesce(fmb.bracenest, fma.bracenest)) AS bracenest,
    Max(Coalesce(fma.bracknest, fmb.bracknest), Coalesce(fmb.bracknest, fma.bracknest)) AS bracknest,
    Max(Coalesce(fma.nuline, fmb.nuline), Coalesce(fmb.nuline, fma.nuline)) AS nuline,
    Max(Coalesce(fma.nppdirective, fmb.nppdirective), Coalesce(fmb.nppdirective, fma.nppdirective)) AS nppdirective,
    Max(Coalesce(fma.nppcond, fmb.nppcond), Coalesce(fmb.nppcond, fma.nppcond)) AS nppcond,
    Max(Coalesce(fma.nppfmacro, fmb.nppfmacro), Coalesce(fmb.nppfmacro, fma.nppfmacro)) AS nppfmacro,
    Max(Coalesce(fma.nppomacro, fmb.nppomacro), Coalesce(fmb.nppomacro, fma.nppomacro)) AS nppomacro,
    Max(Coalesce(fma.ntoken, fmb.ntoken), Coalesce(fmb.ntoken, fma.ntoken)) AS ntoken,
    Max(Coalesce(fma.nstmt, fmb.nstmt), Coalesce(fmb.nstmt, fma.nstmt)) AS nstmt,
    Max(Coalesce(fma.nop, fmb.nop), Coalesce(fmb.nop, fma.nop)) AS nop,
    Max(Coalesce(fma.nuop, fmb.nuop), Coalesce(fmb.nuop, fma.nuop)) AS nuop,
    Max(Coalesce(fma.nnconst, fmb.nnconst), Coalesce(fmb.nnconst, fma.nnconst)) AS nnconst,
    Max(Coalesce(fma.nclit, fmb.nclit), Coalesce(fmb.nclit, fma.nclit)) AS nclit,
    Max(Coalesce(fma.nstring, fmb.nstring), Coalesce(fmb.nstring, fma.nstring)) AS nstring,
    Max(Coalesce(fma.nppconcatop, fmb.nppconcatop), Coalesce(fmb.nppconcatop, fma.nppconcatop)) AS nppconcatop,
    Max(Coalesce(fma.nppstringop, fmb.nppstringop), Coalesce(fmb.nppstringop, fma.nppstringop)) AS nppstringop,
    Max(Coalesce(fma.nif, fmb.nif), Coalesce(fmb.nif, fma.nif)) AS nif,
    Max(Coalesce(fma.nelse, fmb.nelse), Coalesce(fmb.nelse, fma.nelse)) AS nelse,
    Max(Coalesce(fma.nswitch, fmb.nswitch), Coalesce(fmb.nswitch, fma.nswitch)) AS nswitch,
    Max(Coalesce(fma.ncase, fmb.ncase), Coalesce(fmb.ncase, fma.ncase)) AS ncase,
    Max(Coalesce(fma.ndefault, fmb.ndefault), Coalesce(fmb.ndefault, fma.ndefault)) AS ndefault,
    Max(Coalesce(fma.nbreak, fmb.nbreak), Coalesce(fmb.nbreak, fma.nbreak)) AS nbreak,
    Max(Coalesce(fma.nfor, fmb.nfor), Coalesce(fmb.nfor, fma.nfor)) AS nfor,
    Max(Coalesce(fma.nwhile, fmb.nwhile), Coalesce(fmb.nwhile, fma.nwhile)) AS nwhile,
    Max(Coalesce(fma.ndo, fmb.ndo), Coalesce(fmb.ndo, fma.ndo)) AS ndo,
    Max(Coalesce(fma.ncontinue, fmb.ncontinue), Coalesce(fmb.ncontinue, fma.ncontinue)) AS ncontinue,
    Max(Coalesce(fma.ngoto, fmb.ngoto), Coalesce(fmb.ngoto, fma.ngoto)) AS ngoto,
    Max(Coalesce(fma.nreturn, fmb.nreturn), Coalesce(fmb.nreturn, fma.nreturn)) AS nreturn,
    Max(Coalesce(fma.nasm, fmb.nasm), Coalesce(fmb.nasm, fma.nasm)) AS nasm,
    Max(Coalesce(fma.ntypeof, fmb.ntypeof), Coalesce(fmb.ntypeof, fma.ntypeof)) AS ntypeof,
    Max(Coalesce(fma.npid, fmb.npid), Coalesce(fmb.npid, fma.npid)) AS npid,
    Max(Coalesce(fma.nfid, fmb.nfid), Coalesce(fmb.nfid, fma.nfid)) AS nfid,
    Max(Coalesce(fma.nmid, fmb.nmid), Coalesce(fmb.nmid, fma.nmid)) AS nmid,
    Max(Coalesce(fma.nid, fmb.nid), Coalesce(fmb.nid, fma.nid)) AS nid,
    Max(Coalesce(fma.nupid, fmb.nupid), Coalesce(fmb.nupid, fma.nupid)) AS nupid,
    Max(Coalesce(fma.nufid, fmb.nufid), Coalesce(fmb.nufid, fma.nufid)) AS nufid,
    Max(Coalesce(fma.numid, fmb.numid), Coalesce(fmb.numid, fma.numid)) AS numid,
    Max(Coalesce(fma.nuid, fmb.nuid), Coalesce(fmb.nuid, fma.nuid)) AS nuid,
    Max(Coalesce(fma.nlabel, fmb.nlabel), Coalesce(fmb.nlabel, fma.nlabel)) AS nlabel,
    Max(Coalesce(fma.nmacroexpandtoken, fmb.nmacroexpandtoken), Coalesce(fmb.nmacroexpandtoken, fma.nmacroexpandtoken)) AS nmacroexpandtoken,
    Max(Coalesce(fma.ngnsoc, fmb.ngnsoc), Coalesce(fmb.ngnsoc, fma.ngnsoc)) AS ngnsoc,
    Max(Coalesce(fma.nmparam, fmb.nmparam), Coalesce(fmb.nmparam, fma.nmparam)) AS nmparam,
    Max(Coalesce(fma.nfparam, fmb.nfparam), Coalesce(fmb.nfparam, fma.nfparam)) AS nfparam,
    Max(Coalesce(fma.neparam, fmb.neparam), Coalesce(fmb.neparam, fma.neparam)) AS neparam,
    Max(Coalesce(fma.fanin, fmb.fanin), Coalesce(fmb.fanin, fma.fanin)) AS fanin,
    Max(Coalesce(fma.fanout, fmb.fanout), Coalesce(fmb.fanout, fma.fanout)) AS fanout,
    Max(Coalesce(fma.ccycl1, fmb.ccycl1), Coalesce(fmb.ccycl1, fma.ccycl1)) AS ccycl1,
    Max(Coalesce(fma.ccycl2, fmb.ccycl2), Coalesce(fmb.ccycl2, fma.ccycl2)) AS ccycl2,
    Max(Coalesce(fma.ccycl3, fmb.ccycl3), Coalesce(fmb.ccycl3, fma.ccycl3)) AS ccycl3,
    Max(Coalesce(fma.cstruc, fmb.cstruc), Coalesce(fmb.cstruc, fma.cstruc)) AS cstruc,
    Max(Coalesce(fma.chal, fmb.chal), Coalesce(fmb.chal, fma.chal)) AS chal,
    Max(Coalesce(fma.iflow, fmb.iflow), Coalesce(fmb.iflow, fma.iflow)) AS iflow
  -- Pair if possible
  FROM fma FULL OUTER JOIN fmb
    ON fma.gfunctionid = fmb.gfunctionid AND fma.precpp = fmb.precpp;

DROP TABLE functionmetrics;
DROP TABLE fma;
DROP TABLE fmb;

ALTER TABLE new_functionmetrics RENAME TO functionmetrics;
