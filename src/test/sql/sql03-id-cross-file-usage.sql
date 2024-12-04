-- Number of different files that use a given identifier

SELECT name, Count(*) AS cf FROM (
  SELECT fid, tokens.eid, Count(*) AS c
    FROM tokens
    GROUP BY eid, fid
  ) AS cl
  INNER JOIN ids ON cl.eid = ids.eid
  GROUP BY ids.eid, ids.name
  HAVING cf > 1
  ORDER BY cf DESC, name;
