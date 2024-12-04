-- Number of C preprocessor directives in the project's source code files excluding the CScout configuration file

SELECT Sum(nppdirective) FROM filemetrics
  WHERE precpp
    AND fid != (SELECT fid FROM files WHERE name LIKE '%.cs');
