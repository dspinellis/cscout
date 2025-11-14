-- Give predictable numbers to pointer results of tokens-split RDBU eids
-- Remove eid comments to see the assigned eid pointer values

WITH eid_first_offsets AS (
  SELECT
      fid,
      foffset,
      -- eid
      Min(foffset) OVER (PARTITION BY eid) AS rank_offset
      FROM tokens
),
numbered_eids AS (
  SELECT
      fid,
      foffset,
      -- eid,
      Dense_rank() OVER (ORDER BY rank_offset) - 1 AS eid_rank
    FROM eid_first_offsets
)
SELECT
    fid,
    foffset,
    -- eid
    eid_rank AS new_eid
  FROM numbered_eids
  ORDER BY foffset;
