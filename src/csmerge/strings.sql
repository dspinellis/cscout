-- Add to strings missing file records

INSERT INTO strings
  SELECT
    fgm.global_fid AS fid,
    fma.foffset,
    fma.string
  FROM
    adb.strings AS fma
    INNER JOIN fileid_to_global_map AS fgm ON fgm.dbid = 5 AND fgm.fid = fma.fid
    LEFT JOIN strings AS fmb ON fmb.fid = fgm.global_fid
      AND fmb.foffset = fma.foffset
  WHERE fmb.fid IS NULL;
