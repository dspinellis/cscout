-- Create a map from each database's local equivalence class identifiers to
-- identifiers assigned for the corresponding tokens/offsets across all
-- databases.  To do this merge ECs that share tokens.

-- Empty the table to recreate it keeping a backup copy for lookups
DROP TABLE IF EXISTS old_eid_to_global_map;
ALTER TABLE eid_to_global_map RENAME TO old_eid_to_global_map;
CREATE INDEX IF NOT EXISTS idx_old_eid_to_global_map ON old_eid_to_global_map(global_eid);

-- When changing also update merge.sh
CREATE TABLE eid_to_global_map(
  dbid INTEGER,         -- Unmerged database identifier
  eid INTEGER,          -- Equivalence class identifier in an unmerged database
  global_eid INTEGER,   -- Corresponding eid used across all databases
  PRIMARY KEY(dbid, eid)
);
CREATE INDEX IF NOT EXISTS idx_eid_to_global_map ON eid_to_global_map(global_eid);

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
  SELECT * FROM left_joined_tokens
  UNION
  SELECT * FROM right_joined_tokens
),
distinct_ec_pairs AS (
  SELECT DISTINCT * FROM ec_pairs
),
-- Fill in the null values from paired matches
filled_ec_pairs AS (
  SELECT DISTINCT
      Coalesce(half.teid, tp.teid) AS teid,
      Coalesce(half.aeid, ap.aeid) AS aeid
    FROM distinct_ec_pairs AS half
    LEFT JOIN distinct_ec_pairs AS ap
      ON half.teid = ap.teid AND ap.aeid IS NOT NULL
    LEFT JOIN distinct_ec_pairs AS tp
      ON half.aeid = tp.aeid AND tp.teid IS NOT NULL
),
-- Number groups of non-matching ECs.  ECs can match either on the
-- original token side or the attached token side.
teid_changed_rows AS (
  SELECT
      teid,
      aeid,
      CASE
        WHEN
          teid != LAG(teid) OVER (ORDER BY teid)
          OR LAG(teid) OVER (ORDER BY teid) IS NULL
          THEN 1
        ELSE 0
      END AS change_flag
    FROM filled_ec_pairs
),
-- Assign group numbers according to the changes
teid_groups AS (
  SELECT
      teid,
      aeid,
      SUM(change_flag) OVER (ORDER BY teid ROWS UNBOUNDED PRECEDING)
        AS teid_group_id
    FROM teid_changed_rows
),
start_aeid_group_id AS (
  SELECT Max(teid_group_id) + 1 AS value FROM teid_groups
),
aeid_changed_rows AS (
  SELECT
      teid,
      teid_group_id,
      aeid,
      CASE
        WHEN
          aeid != LAG(aeid) OVER (ORDER BY aeid)
          OR LAG(aeid) OVER (ORDER BY aeid) IS NULL
          THEN 1
        ELSE 0
      END AS change_flag
    FROM teid_groups
),
-- Assign group numbers according to the changes
aeid_groups AS (
  SELECT
      teid,
      teid_group_id,
      aeid,
      SUM(change_flag) OVER (ORDER BY aeid ROWS UNBOUNDED PRECEDING)
        + (SELECT value FROM start_aeid_group_id)
        AS aeid_group_id
    FROM aeid_changed_rows
),
-- Provide previous and next group ids, needed to the grouping
-- for the attached and original tokens.
window_values AS (
  SELECT
      aeid,
      aeid_group_id AS curr_ag,
      LAG(aeid_group_id) OVER (ORDER BY aeid_group_id) AS prev_ag,
      LEAD(aeid_group_id) OVER (ORDER BY aeid_group_id) AS next_ag,
      teid,
      teid_group_id AS curr_tg,
      LAG(teid_group_id) OVER (ORDER BY aeid_group_id) AS prev_tg,
      LEAD(teid_group_id) OVER (ORDER BY aeid_group_id) AS next_tg
  FROM aeid_groups
),
-- If tg and ag change provide the value of tg or ag that remains the same
-- in the next row (or ag by default), otherwise set to null to
-- keep the previous value in the next step.
groups_with_nulls AS (
  SELECT aeid, teid,
      CASE
        WHEN (prev_ag != curr_ag AND prev_tg != curr_tg)
            OR prev_ag IS NULL OR prev_tg IS NULL
          THEN CASE
            WHEN curr_tg = next_tg THEN curr_tg
            WHEN curr_ag = next_ag THEN curr_ag
            ELSE curr_tg
          END
        ELSE null
      END as group_id,
      ROW_NUMBER() OVER (ORDER BY curr_ag) AS rn
    FROM window_values
),
groups AS (
  WITH RECURSIVE filled_groups AS (
    SELECT
        rn,
        a.aeid, a.teid,
        group_id
      FROM groups_with_nulls a
      WHERE rn = 1
    UNION ALL
    SELECT
        g.rn,
        g.aeid, g.teid,
        COALESCE(g.group_id, f.group_id) AS group_id
      FROM groups_with_nulls g
      JOIN filled_groups f ON g.rn = f.rn + 1
    )
  SELECT aeid, teid, group_id FROM filled_groups
),
-- Create a map from tokens to their new eids
eid_map AS (
  -- Original tokens
  SELECT DISTINCT om.dbid AS dbid,
      teid AS eid,
      group_id AS global_eid
    FROM groups
    LEFT JOIN old_eid_to_global_map AS om ON om.global_eid = teid
    WHERE teid IS NOT NULL
  UNION ALL
  -- Attached tokens
  SELECT DISTINCT 5 AS dbid,
      aeid AS eid,
      group_id AS global_eid
    FROM groups
    WHERE aeid IS NOT NULL
)
INSERT INTO eid_to_global_map SELECT * from eid_map;

-- Other SELECT statements for troubleshooting using rdbunit
-- To use them, here: comment out the preceding INSERT and DELETE
-- in the rdbu: the INCLUDE CREATE into INCLUDE SELECT and comment
-- out the table name.
-- Example results are in eid_to_global_map.txt.
--SELECT * FROM ec_pairs ORDER BY teid, aeid;
--SELECT * FROM filled_ec_pairs ORDER BY teid, aeid;
--SELECT * FROM teid_changed_rows ORDER BY teid;
--SELECT * FROM teid_groups;
--SELECT * FROM aeid_changed_rows ORDER BY aeid;
--SELECT * FROM aeid_groups;
--SELECT * FROM window_values;
--SELECT * FROM groups;
--SELECT * FROM eid_map;
