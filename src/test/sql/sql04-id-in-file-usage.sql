--  Identifiers occuring ten or more times in a single file

SELECT ids.name AS iname, files.name AS fname, Count(*) AS c
  FROM tokens
  INNER JOIN ids ON ids.eid = tokens.eid
  INNER JOIN files ON tokens.fid = files.fid
  GROUP BY ids.eid, tokens.fid
  HAVING c >= 10
  ORDER BY c DESC, ids.name, files.name;
