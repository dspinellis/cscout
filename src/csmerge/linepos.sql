-- Add to linepos missing file records

INSERT INTO linepos
  SELECT
    fgm.global_fid AS fid,
    fma.foffset,
    fma.lnum
  FROM
    adb.linepos AS fma
    INNER JOIN fileid_to_global_map AS fgm ON fgm.dbid = 5 AND fgm.fid = fma.fid
    LEFT JOIN linepos AS fmb ON fmb.fid = fgm.global_fid
      AND fmb.foffset = fma.foffset
  WHERE fmb.fid IS NULL;
