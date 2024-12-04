-- Macros that cannot be easily converted into C

CREATE INDEX IF NOT EXISTS idx_tokens_eid ON tokens(eid);
CREATE INDEX IF NOT EXISTS idx_functiondefs_functionid ON functiondefs(functionid);
CREATE INDEX IF NOT EXISTS idx_fcalls_destid ON fcalls(destid);
CREATE INDEX IF NOT EXISTS idx_fcalls_sourceid ON fcalls(sourceid);

-- Macros that use local variables
WITH scoped_confused_macros AS (
  SELECT DISTINCT macrodefs.functionid
  FROM functiondefs AS macrodefs
  INNER JOIN functions ON functions.id = macrodefs.functionid
    AND functions.ismacro
  -- Obtain macro's tokens
  INNER JOIN tokens AS macrotokens ON macrotokens.fid = macrodefs.fidbegin
    AND macrotokens.fid = macrodefs.fidend
    AND macrotokens.foffset >= macrodefs.foffsetbegin
    AND macrotokens.foffset < macrodefs.foffsetend
  -- See those that are local objects or labels
  INNER JOIN ids ON ids.eid = macrotokens.eid
    AND ids.ordinary
    AND NOT (ids.macro OR ids.macroarg OR ids.suetag OR ids.sumember OR ids.typedef OR ids.enum)
    AND NOT ids.cscope
    AND NOT ids.lscope
  INNER JOIN files ON macrodefs.fidbegin = files.fid
  -- For macros that get called by C code
  WHERE EXISTS (
      SELECT 1
        FROM fcalls
        WHERE fcalls.destid = macrodefs.functionid
          AND EXISTS (
            SELECT 1
              FROM functions AS cfunctions
              WHERE
                fcalls.sourceid = cfunctions.id
                AND cfunctions.defined
                AND NOT cfunctions.ismacro
          )
          -- And tokens that also appear in the C function body
          AND EXISTS (
            SELECT 1
              FROM functiondefs AS cdefs
              INNER JOIN tokens AS ctokens ON ctokens.eid = macrotokens.eid
              WHERE
                fcalls.sourceid = cdefs.functionid
                AND (cdefs.fidbegin != macrodefs.fidbegin
                  OR cdefs.foffsetbegin > macrodefs.foffsetbegin)
                AND ctokens.fid = cdefs.fidend
                AND ctokens.foffset >= cdefs.foffsetbegin
                AND ctokens.foffset < cdefs.foffsetend
          )
    )
)
SELECT Count(*) FROM
  functions
  INNER JOIN filemetrics ON filemetrics.fid = functions.fid AND filemetrics.precpp
  INNER JOIN functionmetrics AS fnmet ON functions.id = fnmet.functionid AND fnmet.precpp
WHERE ismacro
  AND filemetrics.fid != (SELECT fid FROM files WHERE name LIKE '%.cs')
  AND (
  fnmet.fanin = 0
  OR fnmet.bracenest != 0
  OR fnmet.bracknest != 0
  OR fnmet.nppstringop > 0
  OR fnmet.nppconcatop > 0
  OR fnmet.neparam > 0
  -- Control flow problems
  OR (
    -- Callable macro, rather than macro defining a function
    fnmet.fanin > 0
    AND ((fnmet.ngoto > 0 AND fnmet.nlabel = 0)
      OR (fnmet.nreturn > 1 AND fnmet.ndo = 1))
  )
  OR EXISTS (
    SELECT 1
      FROM scoped_confused_macros AS scm
      WHERE scm.functionid = fnmet.functionid
    )
  )
;
