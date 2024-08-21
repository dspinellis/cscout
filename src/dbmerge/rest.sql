-- Add to rest missing file records

INSERT INTO rest
  SELECT
    fgm.global_fid AS fid,
    fma.foffset,
    fma.code
  FROM
    adb.rest AS fma
    LEFT JOIN fileid_to_global_map AS fgm ON fgm.dbid = 5 AND fgm.fid = fma.fid
    LEFT JOIN rest AS fmb ON fmb.fid = fgm.global_fid
      AND fmb.foffset = fma.foffset
  WHERE fmb.fid IS NULL;
