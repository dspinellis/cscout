-- Create global tokens table with suitably mapped existing entries
-- containing the merged value of the existing and the new ones.
-- The eid_to_global_map table remaps all ECs, so all records have to be
-- created anew.

CREATE TABLE new_ids(
  eid BIGINT PRIMARY KEY, -- Unique identifier key
  name CHARACTER VARYING, -- Identifier name
  readonly BOOLEAN, -- True if it appears in at least one read-only file
  undefmacro BOOLEAN, -- True if it is apparantly an undefined macro
  macro BOOLEAN, -- True if it a preprocessor macro
  macroarg BOOLEAN, -- True if it a preprocessor macro argument
  ordinary BOOLEAN, -- True if it is an ordinary identifier (variable or function)
  suetag BOOLEAN, -- True if it is a structure, union, or enumeration tag
  sumember BOOLEAN, -- True if it is a structure or union member
  label BOOLEAN, -- True if it is a label
  typedef BOOLEAN, -- True if it is a typedef
  enum BOOLEAN, -- True if it is an enumeration member
  yacc BOOLEAN, -- True if it is a yacc identifier
  fun BOOLEAN, -- True if it is a function name
  cscope BOOLEAN, -- True if its scope is a compilation unit
  lscope BOOLEAN, -- True if it has linkage scope
  unused BOOLEAN -- True if it is not used
);

INSERT INTO new_ids
  SELECT
    Coalesce(egma.global_eid, egmb.global_eid) AS eid,
    Coalesce(fma.name, fmb.name) AS name,
    Coalesce(fma.readonly, fmb.readonly) OR Coalesce(fmb.readonly, fma.readonly) AS readonly,
    Coalesce(fma.undefmacro, fmb.undefmacro) AND Coalesce(fmb.undefmacro, fma.undefmacro) AS undefmacro,
    Coalesce(fma.macro, fmb.macro) OR Coalesce(fmb.macro, fma.macro) AS macro,
    Coalesce(fma.macroarg, fmb.macroarg) OR Coalesce(fmb.macroarg, fma.macroarg) AS macroarg,
    Coalesce(fma.ordinary, fmb.ordinary) OR Coalesce(fmb.ordinary, fma.ordinary) AS ordinary,
    Coalesce(fma.suetag, fmb.suetag) OR Coalesce(fmb.suetag, fma.suetag) AS suetag,
    Coalesce(fma.sumember, fmb.sumember) OR Coalesce(fmb.sumember, fma.sumember) AS sumember,
    Coalesce(fma.label, fmb.label) OR Coalesce(fmb.label, fma.label) AS label,
    Coalesce(fma.typedef, fmb.typedef) OR Coalesce(fmb.typedef, fma.typedef) AS typedef,
    Coalesce(fma.enum, fmb.enum) OR Coalesce(fmb.enum, fma.enum) AS enum,
    Coalesce(fma.yacc, fmb.yacc) OR Coalesce(fmb.yacc, fma.yacc) AS yacc,
    Coalesce(fma.fun, fmb.fun) OR Coalesce(fmb.fun, fma.fun) AS fun,
    Coalesce(fma.cscope, fmb.cscope) OR Coalesce(fmb.cscope, fma.cscope) AS cscope,
    Coalesce(fma.lscope, fmb.lscope) OR Coalesce(fmb.lscope, fma.lscope) AS lscope,
    Coalesce(fma.unused, fmb.unused) AND Coalesce(fmb.unused, fma.unused) AS unused
  FROM
    adb.ids AS fma
    LEFT JOIN eid_to_global_map AS egma
      ON egma.dbid = 5 AND egma.eid = fma.eid
    -- Requires SQLite >= 3.39.0
    FULL OUTER JOIN ids AS fmb
      ON fma.eid = fmb.eid
    LEFT JOIN eid_to_global_map AS egmb
      ON egmb.dbid != 5 AND egmb.eid = fmb.eid;

DROP TABLE IF EXISTS ids;
ALTER TABLE new_ids RENAME TO ids;
