-- Identifiers common between files participating in a define/use relationship

SELECT
    min(ids.name) as identifier,
    min(filesb.name) as defined,
    min(filesa.name) as used
  FROM definers
  INNER JOIN tokens AS tokensa ON definers.basefileid = tokensa.fid
  INNER JOIN tokens AS tokensb ON definers.definerid = tokensb.fid
  INNER JOIN ids ON ids.eid = tokensa.eid
  INNER JOIN files as filesa ON tokensa.fid = filesa.fid
  INNER JOIN files as filesb ON tokensb.fid = filesb.fid
  WHERE tokensa.eid = tokensb.eid
  GROUP BY tokensa.eid, definerid, basefileid
  ORDER BY defined, identifier, used;
