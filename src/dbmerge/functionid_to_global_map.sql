-- Create a map from each database's local function identifiers to
-- identifiers assigned for the corresponding functions across all
-- databases.

CREATE INDEX IF NOT EXISTS adb.idx_functions_composite ON functions(fid, foffset);

WITH max_id AS (
  SELECT IFNULL(MAX(global_id), 0) AS max_id FROM functionid_to_global_map
),

joined_functions AS (
    -- Local and global fids
    SELECT afunctions.id AS id, functions.id AS global_id
      FROM adb.functions AS afunctions
      LEFT JOIN fileid_to_global_map AS fidmap
        ON fidmap.dbid = 5 AND fidmap.fid = afunctions.fid
      LEFT JOIN functions ON functions.fid = fidmap.global_fid
        AND functions.foffset = afunctions.foffset
        AND functions.name = afunctions.name
)
INSERT INTO functionid_to_global_map
    -- Functions already mapped
    SELECT 5 AS dbid,
        id,
        global_id
      FROM joined_functions
      WHERE global_id IS NOT NULL
  UNION ALL
    -- Newly mapped functions
    SELECT 5 AS dbid,
      id,
      max_id.max_id + ROW_NUMBER() OVER () AS global_id
    FROM joined_functions
    CROSS JOIN max_id
    WHERE global_id IS NULL;
