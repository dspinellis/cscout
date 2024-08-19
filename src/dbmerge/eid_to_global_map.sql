-- Create a map from each database's local equivalence class identifiers to
-- identifiers assigned for the corresponding tokens/offsets across all
-- databases.  To do this merge ECs that share tokens.

CREATE INDEX IF NOT EXISTS idx_eid_to_global_map ON eid_to_global_map(global_eid);

-- When changing also update merge.sh
CREATE TABLE new_eid_to_global_map(
  dbid INTEGER,         -- Unmerged database identifier
  eid INTEGER,          -- Equivalence class identifier in an unmerged database
  global_eid INTEGER,   -- Corresponding eid used across all databases
  PRIMARY KEY(dbid, eid)
);

DROP TABLE IF EXISTS ec_pairs;

CREATE TABLE ec_pairs AS SELECT * FROM (
  -- Map attached tokens (having aeid ECs) to use the global fid
  WITH mapped_atokens AS (
    SELECT fid_map.global_fid AS fid, at.foffset, at.eid
      FROM adb.tokens AS at
      LEFT JOIN fileid_to_global_map AS fid_map
        ON fid_map.dbid = 5 AND fid_map.fid = at.fid
  ),
  -- Map each original token with any matching attached tokens
  left_joined_tokens AS (
    SELECT DISTINCT tokens.eid AS teid, atokens.eid AS aeid
      FROM tokens
      LEFT JOIN mapped_atokens AS atokens ON tokens.fid = atokens.fid
        AND tokens.foffset = atokens.foffset
  ),
  -- Map each attached token with any matching original tokens
  right_joined_tokens AS (
    SELECT DISTINCT tokens.eid AS teid, atokens.eid AS aeid
      FROM mapped_atokens AS atokens
      LEFT JOIN tokens ON tokens.fid = atokens.fid
        AND tokens.foffset = atokens.foffset
  ),
  -- Combine the two to have all the possible mappings
  ec_pairs AS (
    SELECT teid, aeid FROM left_joined_tokens
    UNION
    SELECT teid, aeid FROM right_joined_tokens
  )
  SELECT DISTINCT teid, aeid FROM ec_pairs
);

-- Create a GraphViz file; use -5 to get a unique name
.output ec_pairs-5.gv
SELECT 'graph G {';
-- Map attached tokens (having aeid ECs) to use the global fid
WITH joined_tokens AS (
  SELECT teid, aeid
    FROM ec_pairs
    WHERE teid IS NOT NULL and aeid IS NOT NULL
)
-- Nodes
SELECT
    ' "n'
    || teid
    || '" [T=t]; "n'
    || aeid
    || '" [T=a];'
  FROM joined_tokens
UNION
-- Edges
SELECT
    ' "n'
    || teid
    || '" -- "n'
    || aeid
    || '";'
  FROM joined_tokens;
SELECT '}';
.output stdout

-- Convert the graph into a CSV file of token groups
.shell ccomps ec_pairs-5.gv | gvtocsv.awk >token_groups-5.csv

DROP TABLE IF EXISTS token_groups;
CREATE TABLE token_groups(
  eid INTEGER,
  eid_type CHAR,
  global_eid INTEGER,
  PRIMARY KEY(eid, eid_type)
);

.mode csv
.import token_groups-5.csv token_groups

-- Create a map from tokens to their new eids
WITH new_global_eid AS (
  SELECT Max(global_eid) + 1 AS value FROM token_groups
),
eid_map AS (
  -- Original tokens in components with others
  SELECT om.dbid AS dbid,
      token_groups.eid,
      token_groups.global_eid
    FROM token_groups
    LEFT JOIN eid_to_global_map AS om
      ON om.global_eid = token_groups.eid
    WHERE eid_type = 't'
  UNION
  -- Original tokens not joined in a component
  -- Use their eid incremented by the highest assigned global_eid
  SELECT om.dbid AS dbid,
      ec_pairs.teid AS eid,
      ec_pairs.teid + (SELECT value FROM new_global_eid) AS global_eid
    FROM ec_pairs
    LEFT JOIN token_groups
      ON token_groups.eid = ec_pairs.teid AND token_groups.eid_type = 't'
    LEFT JOIN eid_to_global_map AS om
      ON om.global_eid = ec_pairs.teid
    WHERE ec_pairs.teid IS NOT NULL AND token_groups.global_eid IS NULL
  UNION
  -- Attached tokens in components with others
  SELECT 5 AS dbid,
      token_groups.eid,
      token_groups.global_eid
    FROM token_groups
    WHERE eid_type = 'a'
  UNION
  -- Attached tokens not joined in a component
  -- Use their eid incremented by the highest assigned global_eid
  SELECT 5 AS dbid,
      ec_pairs.aeid AS eid,
      ec_pairs.aeid + (SELECT value FROM new_global_eid) AS global_eid
    FROM ec_pairs
    LEFT JOIN token_groups
      ON token_groups.eid = ec_pairs.aeid AND token_groups.eid_type = 'a'
    WHERE ec_pairs.aeid IS NOT NULL AND token_groups.global_eid IS NULL
)
INSERT INTO new_eid_to_global_map SELECT * from eid_map;

DROP TABLE IF EXISTS eid_to_global_map;
ALTER TABLE new_eid_to_global_map RENAME TO eid_to_global_map;

-- Drop temporary tables
DROP TABLE ec_pairs;
DROP TABLE token_groups;
