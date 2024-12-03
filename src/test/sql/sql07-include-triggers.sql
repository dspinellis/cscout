-- Show the included files required by other files for each compilation unit and project.

SELECT
    projects.name AS projname,
    cufiles.name AS cuname,
    basefiles.name AS basename,
    definefiles.name AS defname
  FROM definers
  INNER join projects ON definers.pid = projects.pid
  INNER JOIN files AS cufiles ON definers.cuid=cufiles.fid
  INNER JOIN  files AS basefiles ON definers.basefileid=basefiles.fid
  INNER JOIN files AS definefiles ON definers.definerid = definefiles.fid;
