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
local_functions AS (
  SELECT functions.id, functions.name, tokens.fid, tokens.foffset
    FROM functions
    LEFT JOIN functionid
      ON functions.id = functionid.functionid AND functionid.ordinal = 0
    LEFT JOIN tokens
      ON functionid.eid = tokens.eid
),

attached_functions AS (
  SELECT afunctions.id, afunctions.name, atokens.fid, atokens.foffset
    FROM adb.functions AS afunctions
    LEFT JOIN adb.functionid AS afunctionid
      ON afunctions.id = afunctionid.functionid AND afunctionid.ordinal = 0
    LEFT JOIN adb.tokens AS atokens
      ON afunctionid.eid = atokens.eid
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
