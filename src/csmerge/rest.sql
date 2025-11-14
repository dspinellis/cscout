-- Add to rest missing file records.
-- In some merged files identifiers may appear in code in others in tokens.
-- As some existing elements may be removed, start with a fresh table.
--
-- The employed algorithm is a good approximation: it works correctly in 1221
-- out of 1222 tested files.  However, it may fail when a file
-- is conditionally compiled in different ways in two merged files,
-- e.g. /home/dds/src/linux-v6.14.2/include/trace/define_trace.h:89-92
-- (see debugging notes).
-- A better algorithm is as follows:
-- * Order records by file and offset.
-- * When they match select the one from the first file.
-- * When they don't match select the shortest one, and continue.
--   selecting elements from the same file, until they match again.
-- * When done, remove tokens that fall within the selected code elements.
--

CREATE TABLE new_rest(
  fid INTEGER, -- File key
  foffset INTEGER, -- Offset within the file
  code CHARACTER VARYING, -- The actual code
  PRIMARY KEY(fid, foffset)
);

-- Elements from both rest sides
WITH all_rest AS (
  SELECT 1 AS dbid,
      fgm.global_fid AS fid,
      fma.foffset,
      fma.code
    FROM adb.rest AS fma
    INNER JOIN fileid_to_global_map AS fgm ON fgm.dbid = 5 AND fgm.fid = fma.fid
  UNION ALL
  SELECT 0 AS dbid, rest.*
    FROM rest
),
-- Match them with numbered rows
ranked_partitioned AS
(
  SELECT *,
         ROW_NUMBER() OVER (
             PARTITION BY fid, foffset
             ORDER BY Length(code) ASC, dbid ASC
         ) AS rn
  FROM all_rest
)
INSERT INTO new_rest
  SELECT fid, foffset, code
    FROM ranked_partitioned
    -- Give precedence to shorter elements
    WHERE rn = 1;

DROP TABLE rest;
ALTER TABLE new_rest RENAME TO rest;

-- Eliminate the few records that have been superseded by matched tokens
-- Do it here to use the table indices
WITH matched_rows AS (
  SELECT rest.fid, rest.foffset
    FROM rest
    INNER JOIN tokens ON rest.fid = tokens.fid AND rest.foffset = tokens.foffset
)
DELETE FROM rest
  WHERE (fid, foffset) IN (
      SELECT fid, foffset FROM matched_rows
);

