-- Create a map from each database's local equivalence class identifiers to
-- identifiers assigned for the corresponding tokens/offsets across all
-- databases.

WITH max_id AS (
  SELECT IFNULL(MAX(global_eid), 0) AS max_id FROM eid_to_global_map
),
joined_tokens AS (
    -- Local and global fids
    SELECT DISTINCT atokens.eid AS eid, tokens.eid AS global_eid
      FROM adb.tokens AS atokens
      LEFT JOIN fileid_to_global_map AS fid_map
	ON fid_map.dbid = 5 AND fid_map.fid = atokens.fid
      LEFT JOIN tokens ON tokens.fid = fid_map.global_fid
        AND tokens.foffset = atokens.foffset
),
-- Add found EC to same but non-matched ECs
-- Eg. (52, 2), (52, null), (51, null) becomes
--     (52, 2), (51, null)
distinct_ecs AS (
  SELECT a.* FROM joined_tokens AS a
    LEFT JOIN joined_tokens AS b ON a.eid = b.eid
      AND b.global_eid IS NOT NULL
    WHERE a.global_eid IS NOT NULL
      OR b.eid IS NULL
)
INSERT INTO eid_to_global_map
    -- ECs already mapped
    SELECT 5 AS dbid,
        eid,
        global_eid
      FROM distinct_ecs
      WHERE global_eid IS NOT NULL
  UNION
    -- Newly mapped tokens
    SELECT 5 AS dbid,
      eid,
      max_id.max_id + ROW_NUMBER() OVER () AS global_eid
    FROM distinct_ecs
    CROSS JOIN max_id
    WHERE global_eid IS NULL;
