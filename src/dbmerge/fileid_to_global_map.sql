-- Create a map from each database's local file identifiers to
-- identifiers assigned for the corresponding files across all
-- databases.

CREATE INDEX IF NOT EXISTS adb.idx_files_name ON files(name);

WITH max_id AS (
  SELECT IFNULL(MAX(global_fid), 0) AS max_id FROM fileid_to_global_map
),
joined_files AS (
    -- Local and global fids
    SELECT afiles.fid AS fid, files.fid AS global_fid
      FROM adb.files AS afiles
      LEFT JOIN files ON files.name = afiles.name
)
INSERT INTO fileid_to_global_map
    -- Files already mapped
    SELECT 5 AS dbid,
        fid,
        global_fid
      FROM joined_files
      WHERE global_fid IS NOT NULL
  UNION
    -- Newly mapped files
    SELECT 5 AS dbid,
      fid,
      max_id.max_id + ROW_NUMBER() OVER () AS global_fid
    FROM joined_files
    CROSS JOIN max_id
    WHERE global_fid IS NULL;
