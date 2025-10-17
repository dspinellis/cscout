-- Merge elements that are identified by equivalence class (EC) ids (eids).
-- This requires running CScout's token homogenization alrogithm
-- by writing out tables identifying ECs by fid/foffset, invoking CScout
-- to merge them, and then reading them back.

-- A map from eids to an identifying tokid
CREATE TEMP TABLE eid_to_tokid_map(
  eid INTEGER PRIMARY KEY,-- Equivalence class identifier in an unmerged database
  fid INTEGER,          -- File id representing eid
  foffset INTEGER       -- File offset representing eid
);

WITH ranked_rows AS (
  SELECT eid, fid, foffset,
      ROW_NUMBER() OVER (PARTITION BY eid ORDER BY (SELECT NULL)) AS rn
  FROM tokens
)
INSERT INTO eid_to_tokid_map
  SELECT eid, fid, foffset
    FROM ranked_rows
    WHERE rn = 1;

-- Same for attached database
CREATE TEMP TABLE aeid_to_tokid_map(
  eid INTEGER PRIMARY KEY,-- Equivalence class identifier in an unmerged database
  fid INTEGER,          -- File id representing eid
  foffset INTEGER       -- File offset representing eid
);

WITH ranked_rows AS (
  SELECT eid, fid, foffset,
      ROW_NUMBER() OVER (PARTITION BY eid ORDER BY (SELECT NULL)) AS rn
  FROM adb.tokens
),
eid_sentinel_tokens AS (
  SELECT eid, fid, foffset
    FROM ranked_rows
    WHERE rn = 1
)
INSERT INTO aeid_to_tokid_map
  SELECT eid, fid_map.global_fid AS fid, foffset
    FROM eid_sentinel_tokens AS est
    LEFT JOIN fileid_to_global_map AS fid_map
      ON fid_map.dbid = 5 AND fid_map.fid = est.fid;

.separator ' '
-- Start with the attached database where Eclasses are created more efficiently.
-- When creating new databases the attached is by definition larger than the
-- (empty) original one.
-- This is read by read_eclasses_attached.
.output eclasses-a-5.txt
SELECT fid_map.global_fid AS fid, foffset, length(name) AS len, eid
  FROM adb.tokens AS at
  LEFT JOIN fileid_to_global_map AS fid_map
    ON fid_map.dbid = 5 AND fid_map.fid = at.fid
  LEFT JOIN adb.ids USING(eid)
  ORDER BY eid;

-- Continue with the original classes, which must be processed twice.
-- This is read by read_eclasses_original.
.output eclasses-o-5.txt
SELECT fid, foffset, length(name) AS len, eid
  FROM tokens
  LEFT JOIN ids USING(eid)
  ORDER BY eid;

.output ids-5.txt
SELECT 0 AS dbid, fid, foffset, ids.*
  FROM ids
  LEFT JOIN eid_to_tokid_map USING(eid);

SELECT 5 AS dbid, fid, foffset, ai.*
  FROM adb.ids AS ai
  LEFT JOIN aeid_to_tokid_map USING(eid);

.output functionid-5.txt
  SELECT 5 AS dbid,
        functionid,
        etm.fid,
        etm.foffset,
        length(ids.name) AS len
    FROM adb.functionid AS afunctionid
    LEFT JOIN aeid_to_tokid_map AS etm USING(eid)
    LEFT JOIN adb.ids USING(eid)
    ORDER BY functionid, ordinal;

  SELECT 0 AS dbid,
        functionid,
        etm.fid,
        etm.foffset,
        length(ids.name) AS len
    FROM functionid
    LEFT JOIN eid_to_tokid_map AS etm USING(eid)
    LEFT JOIN ids USING(eid)
    ORDER BY functionid, ordinal;

.output idproj-5.txt
  SELECT etm.fid,
        etm.foffset,
        length(ids.name) AS len,
        pid
    FROM adb.idproj AS idproj
    LEFT JOIN aeid_to_tokid_map AS etm USING(eid)
    LEFT JOIN adb.ids USING(eid);

  SELECT etm.fid,
        etm.foffset,
        length(ids.name) AS len,
        pid
    FROM idproj
    LEFT JOIN eid_to_tokid_map AS etm USING(eid)
    LEFT JOIN ids USING(eid);

.output stdout

-- Invoke CScout to merge and unify the output elements
.shell cscout -M eclasses-a-5.txt eclasses-o-5.txt ids-5.txt functionid-5.txt idproj-5.txt new-eclasses-5.csv new-ids-5.csv new-functionid-5.csv new-idproj-5.csv functionid-to-global-map.csv

DELETE FROM tokens;
DELETE FROM ids;
DELETE FROM functionid;
DELETE FROM idproj;
DELETE FROM functionid_to_global_map;

.mode csv
.import new-eclasses-5.csv tokens
.import new-ids-5.csv ids
.import new-functionid-5.csv functionid
.import new-idproj-5.csv idproj
.import functionid-to-global-map.csv functionid_to_global_map
.mode list

-- Drop temporary tables
DROP TABLE aeid_to_tokid_map;
DROP TABLE eid_to_tokid_map;
