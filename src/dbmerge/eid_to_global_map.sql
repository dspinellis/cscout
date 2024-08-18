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
    SELECT * FROM left_joined_tokens
    UNION
    SELECT * FROM right_joined_tokens
  )
  SELECT DISTINCT * FROM ec_pairs
);

-- Convert teids into small integers to be used as typed node identifiers
DROP TABLE IF EXISTS teid_map;
CREATE TABLE teid_map(teid PRIMARY KEY, teid_node);
INSERT INTO teid_map 
  SELECT teid, ROW_NUMBER() OVER (ORDER BY teid) AS teid_node
    FROM (
      SELECT DISTINCT(teid) AS teid
        FROM ec_pairs
        WHERE teid IS NOT NULL
    );
CREATE INDEX idx_teid_map_teid_node ON teid_map(teid_node);

-- Convert aeids into large integers to be used as typed node identifiers
DROP TABLE IF EXISTS aeid_map;
CREATE TABLE aeid_map(aeid PRIMARY KEY, aeid_node);
INSERT INTO aeid_map 
  SELECT aeid, ROW_NUMBER() OVER (ORDER BY aeid) + 1000000000 AS aeid_node
    FROM (
      SELECT DISTINCT(aeid) AS aeid
        FROM ec_pairs
        WHERE aeid IS NOT NULL
    );
CREATE INDEX idx_aeid_map_aeid_node ON aeid_map(aeid_node);

-- Create a table of undirected mapped edges from ec_pairs
-- to be used in the BFS.
DROP TABLE IF EXISTS edges;
CREATE TABLE edges(a, b, PRIMARY KEY(a, b));
INSERT INTO edges SELECT * FROM (
  WITH mapped_edges AS (
    SELECT teid_map.teid_node, aeid_map.aeid_node
    FROM ec_pairs
    LEFT JOIN teid_map ON ec_pairs.teid = teid_map.teid
    LEFT JOIN aeid_map ON ec_pairs.aeid = aeid_map.aeid
  ),
  all_edges AS (
    SELECT teid_node AS a, aeid_node AS b FROM mapped_edges
    UNION
    SELECT aeid_node AS a, teid_node AS b FROM mapped_edges
  )
  SELECT DISTINCT * FROM all_edges
);

-- Load BFS SQLite extension
-- https://github.com/abetlen/sqlite3-bfsvtab-ext
.load bfsvtab.so

-- Obtain all nodes in the graph
WITH all_nodes AS (
  SELECT a AS id FROM edges
  UNION
  SELECT b AS id FROM edges
),
nodes AS (
  SELECT DISTINCT id FROM all_nodes WHERE id IS NOT NULL
),
-- Visit them with a BFS search
visits AS (
  SELECT 
    nodes.id AS a, bfs.id AS b
    FROM nodes
    LEFT JOIN bfsvtab AS bfs
      ON
        tablename  = 'edges'
        AND fromcolumn = 'a'
        AND tocolumn = 'b'
        AND root = nodes.id
),
-- Find CCs by mapping each visited node with its minimum pair
connected_components AS (
  SELECT a AS node, Min(b) AS component
    FROM visits
    GROUP BY a
),
-- Create a map from tokens to their new eids
eid_map AS (
  -- Original tokens
  SELECT om.dbid AS dbid,
      teid_map.teid AS eid,
      component AS global_eid
    FROM connected_components AS cct
    LEFT JOIN teid_map ON teid_map.teid_node = cct.node
    LEFT JOIN eid_to_global_map AS om
      ON om.global_eid = teid_map.teid
    WHERE node < 1000000000
  UNION ALL
  -- Attached tokens
  SELECT 5 AS dbid,
      aeid_map.aeid AS eid,
      component AS global_eid
    FROM connected_components AS cca
    LEFT JOIN aeid_map ON aeid_map.aeid_node = cca.node
    WHERE node >= 1000000000
)
INSERT INTO new_eid_to_global_map SELECT * from eid_map;

DROP TABLE IF EXISTS eid_to_global_map;
ALTER TABLE new_eid_to_global_map RENAME TO eid_to_global_map;
