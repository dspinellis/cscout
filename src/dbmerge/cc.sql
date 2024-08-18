.load bfsvtab.so

CREATE VIEW undirected_edges AS
  SELECT DISTINCT * FROM (
    SELECT a, b FROM edges
    UNION
    SELECT b AS a, a AS b FROM edges
  );

CREATE TABLE connected_components AS SELECT * FROM (
  WITH all_nodes AS (
    SELECT a AS id FROM edges
    UNION
    SELECT b AS id FROM edges
  ),
  nodes AS (
    SELECT DISTINCT id FROM all_nodes WHERE id IS NOT NULL
  ),
  visits AS (
    SELECT 
      nodes.id AS a, bfs.id AS b
      FROM nodes
      LEFT JOIN bfsvtab AS bfs
        ON
          tablename  = 'undirected_edges'
          AND fromcolumn = 'a'
          AND tocolumn = 'b'
          AND root = nodes.id
  )
  SELECT a AS node, Min(b) AS component
    FROM visits
    GROUP BY a
);
