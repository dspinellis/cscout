--  Identifiers occuring ten or more times in the workspace

SELECT name, Count(*) AS c FROM tokens
  INNER JOIN ids ON ids.eid = tokens.eid
  GROUP BY ids.eid
  HAVING c >= 10
  ORDER BY c DESC, name;
