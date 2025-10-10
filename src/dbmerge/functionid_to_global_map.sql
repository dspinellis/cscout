-- Create a map from each database's local function identifiers to
-- identifiers assigned for the corresponding functions across all
-- databases.

CREATE INDEX IF NOT EXISTS adb.idx_functionid_composite ON functionid(functionid, ordinal);
CREATE INDEX IF NOT EXISTS adb.idx_functions_id ON functions(id);
CREATE INDEX IF NOT EXISTS adb.idx_functionid_eid ON functionid(eid);
CREATE INDEX IF NOT EXISTS adb.idx_tokens_eid ON tokens(eid);

WITH max_id AS (
  SELECT IFNULL(MAX(global_id), 0) AS max_id FROM functionid_to_global_map
),

-- According to CScout's implementation functions are uniquely identified
-- by their first tokid and their name.
-- It is incorrect to match them by fid, foffset stored in functions:
-- search for __do_sys_dup in the CScout source code.
-- Add a fid, foffset to be used as portable identifiers.
local_functions AS (
  SELECT f.id, f.name, t.fid, t.foffset
    FROM functions AS f
    LEFT JOIN functionid AS fi
      ON f.id = fi.functionid AND fi.ordinal = 0
    LEFT JOIN tokens AS t ON t.rowid =
      -- Get a single fid, foffset.
      (SELECT rowid FROM tokens WHERE tokens.eid = fi.eid LIMIT 1)
),

-- Add a fid, foffset to be used as portable identifiers.
attached_functions AS (
  SELECT f.id, f.name, t.fid, t.foffset
    FROM adb.functions AS f
    LEFT JOIN adb.functionid AS fi
      ON f.id = fi.functionid AND fi.ordinal = 0
    LEFT JOIN adb.tokens AS t ON t.rowid =
      -- Get a single fid, foffset.
      (SELECT rowid FROM tokens WHERE tokens.eid = fi.eid LIMIT 1)
),

joined_functions AS (
    -- Local and global fids
    SELECT afunctions.id AS id, lfunctions.id AS global_id
      FROM attached_functions AS afunctions
      LEFT JOIN fileid_to_global_map AS fidmap
        ON fidmap.dbid = 5 AND fidmap.fid = afunctions.fid
      LEFT JOIN local_functions AS lfunctions
        ON lfunctions.fid = fidmap.global_fid
        AND lfunctions.foffset = afunctions.foffset
        AND lfunctions.name = afunctions.name
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
