-- Projects each identifier named main belongs to

SELECT ids.name, projects.name
  FROM ids
  INNER JOIN idproj ON ids.eid = idproj.eid
  INNER JOIN projects ON idproj.pid = projects.pid
  WHERE ids.name = 'main'
  ORDER BY ids.name;

