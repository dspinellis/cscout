-- Create a map from each database's local function identifiers to
-- identifiers assigned for the corresponding functions across all
-- databases.
-- This is done by joining functions of fid/foffset/name.
-- This is a lot more reliable than using the functionid table,
-- because that comingles through merged equivalence classes
-- identifiers of functions that aren't the same, such as
-- __io_bw() in Linux's arch/arm64/include/asm/io.h and
-- arch/riscv/include/asm/mmio.h.

CREATE INDEX IF NOT EXISTS adb.idx_function_composite ON functions(fid, foffset, name);

WITH max_id AS (
  SELECT IFNULL(MAX(global_id), 0) AS max_id FROM functionid_to_global_map
),
joined_functions AS (
    -- Local and global ids
    SELECT afunctions.id AS id, functions.id AS global_id
      FROM adb.functions AS afunctions
      INNER JOIN fileid_to_global_map AS fid_map
        ON fid_map.dbid = 5 AND fid_map.fid = afunctions.fid
      LEFT JOIN functions ON functions.fid = fid_map.global_fid
        AND functions.foffset = afunctions.foffset
        AND functions.name = afunctions.name
)
INSERT INTO functionid_to_global_map
    -- Files already mapped
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
