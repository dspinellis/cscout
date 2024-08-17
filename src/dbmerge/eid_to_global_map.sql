-- Create a map from each database's local equivalence class identifiers to
-- identifiers assigned for the corresponding tokens/offsets across all
-- databases.  To do this merge ECs that share tokens.


--CREATE INDEX IF NOT EXISTS idx_eid_to_global_map ON eid_to_global_map(global_eid);

-- When changing also update merge.sh
--CREATE TABLE new_eid_to_global_map(
--  dbid INTEGER,         -- Unmerged database identifier
--  eid INTEGER,          -- Equivalence class identifier in an unmerged database
--  global_eid INTEGER,   -- Corresponding eid used across all databases
--  PRIMARY KEY(dbid, eid)
--);

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
-- Convert the teid-aeid graph into its connected components
-- First convert ec_pairs into types nodes
all_nodes AS (
  SELECT teid AS node_id, 't' AS node_type
    FROM distinct_ec_pairs
    WHERE teid IS NOT NULL AND aeid IS NOT NULL
  UNION
  SELECT aeid AS node_id, 'a' AS node_type
    FROM distinct_ec_pairs
    WHERE teid IS NULL AND aeid IS NOT NULL
),
-- Calculate their connected components
connected_components AS (
  WITH RECURSIVE components(node_id, node_type, component_id) AS (
    -- Base case: Start with all teid, aeid nodes with an initial component_id
      SELECT 
          node_id, node_type, ROW_NUMBER() OVER () AS component_id
      FROM (SELECT DISTINCT * FROM all_nodes)

      UNION ALL

      -- Recursive case: Expand components by following edges
      SELECT teid AS node_id, 't' AS node_type, component_id
      FROM distinct_ec_pairs 
      JOIN components AS c
        ON c.node_id = distinct_ec_pairs.teid
          AND c.node_type = 't'
      WHERE teid IS NOT NULL
        AND teid NOT IN (
          SELECT node_id FROM components
            WHERE component_id = c.component_id and node_type = 't'
        )

      UNION ALL

      SELECT aeid AS node_id, 'a' AS node_type, component_id
      FROM distinct_ec_pairs 
      JOIN components AS c
        ON c.node_id = distinct_ec_pairs.aeid
          AND c.node_type = 'a'
      WHERE aeid IS NOT NULL
        AND aeid NOT IN (
          SELECT node_id FROM components
            WHERE component_id = c.component_id and node_type = 't'
        )
    )
    SELECT DISTINCT
        node_id,
        node_type,
        MIN(component_id) OVER (PARTITION BY node_id) AS component
      FROM components
),
-- Create a map from tokens to their new eids
eid_map AS (
  -- Original tokens
  SELECT om.dbid AS dbid,
      node_id AS eid,
      component_id AS global_eid
    FROM connected_components
    LEFT JOIN eid_to_global_map AS om ON om.global_eid = teid
    WHERE node_id = 't'
  UNION ALL
  -- Attached tokens
  SELECT 5 AS dbid,
      node_id AS eid,
      component_id AS global_eid
    FROM connected_components
    WHERE node_id = 'a'
)
--INSERT INTO new_eid_to_global_map SELECT * from eid_map;
--DROP TABLE IF EXISTS eid_to_global_map;
--ALTER TABLE new_eid_to_global_map RENAME TO eid_to_global_map;

-- Other SELECT statements for troubleshooting using rdbunit
-- To use them, here: comment out the preceding INSERT, DROP, ALTER
-- Comment out the CREATE TABLE at the file's start
-- In the .rdbu file: change INCLUDE CREATE into INCLUDE SELECT and comment
-- out the table name.
-- Example results are in eid_to_global_map.txt.
--SELECT * FROM ec_pairs ORDER BY teid, aeid;
--SELECT * FROM all_nodes;
SELECT * FROM connected_components;
