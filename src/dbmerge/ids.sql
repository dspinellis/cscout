-- Add to ids missing records, and update any existing ones
-- with the merged value of the existing and the new ones.

INSERT OR REPLACE INTO ids
  SELECT
    egm.global_eid AS eid,
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
    LEFT JOIN eid_to_global_map AS egm ON egm.dbid = 5 AND egm.eid = fma.eid
    LEFT JOIN ids AS fmb ON fmb.eid = egm.global_eid;
