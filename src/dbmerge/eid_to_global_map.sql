-- Create a map from each database's local equivalence class identifiers to
-- identifiers assigned for the corresponding tokens/offsets across all
-- databases.  To do this merge ECs that share tokens.

-- Clear original map as it is useless
DELETE FROM eid_to_global_map;

-- Original database id for existing tokens having teid ECs
WITH original_db_id AS (
  SELECT fm.dbid AS dbid
    FROM fileid_to_global_map AS fm
    LEFT JOIN (SELECT fid FROM tokens LIMIT 1) AS tf
      ON tf.fid = fm.fid
    LIMIT 1
),
-- Map attached tokens (having aeid ECs) to use the global fid
mapped_atokens AS (
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
  SELECT * from left_joined_tokens
  UNION
  SELECT * from right_joined_tokens
),
-- Fill in the null values from paired matches
filled_ec_pairs AS (
  SELECT DISTINCT
      Coalesce(half.teid, tp.teid) AS teid,
      Coalesce(half.aeid, ap.aeid) AS aeid
    FROM ec_pairs AS half
    LEFT JOIN ec_pairs AS ap ON half.teid = ap.teid AND ap.aeid IS NOT NULL
    LEFT JOIN ec_pairs AS tp ON half.aeid = tp.aeid AND tp.teid IS NOT NULL
),
-- Number groups of non-matching ECs.  ECs can match either on the
-- original token side or the attached token side.
changed_rows AS (
  SELECT
      teid, 
      aeid,
      CASE 
        WHEN
          (teid != LAG(teid) OVER (ORDER BY teid, aeid)
            AND aeid != LAG(aeid) OVER (ORDER BY teid, aeid))
          OR LAG(teid) OVER (ORDER BY teid, aeid) IS NULL
          OR LAG(aeid) OVER (ORDER BY teid, aeid) IS NULL
          THEN 1 
        ELSE 0 
      END AS change_flag
    FROM filled_ec_pairs
),
-- Assign group numbers according to the changes
groups AS (
  SELECT
      teid,
      aeid,
      SUM(change_flag) OVER (ORDER BY teid, aeid ROWS UNBOUNDED PRECEDING)
        AS group_id
    FROM changed_rows
),
-- Create a map from tokens to their new eids
eid_map AS (
  -- Original tokens
  SELECT DISTINCT original_db_id.dbid AS dbid,
      teid AS eid,
      group_id AS global_eid
    FROM groups CROSS JOIN original_db_id
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
-- See eid_to_global_map.txt.
--SELECT * FROM filled_ec_pairs ORDER BY teid, aeid;
--SELECT * FROM changed_rows ORDER BY teid, aeid;
--SELECT * FROM groups;
--SELECT * FROM eid_map;
