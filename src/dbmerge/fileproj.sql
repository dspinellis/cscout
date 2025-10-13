-- Add to fileproj missing records

INSERT INTO fileproj
  SELECT
    fgm.global_fid AS fid,
    ipa.pid
  FROM adb.fileproj AS ipa
    LEFT JOIN fileid_to_global_map AS fgm
      ON fgm.dbid = 5 AND fgm.fid = ipa.fid
    LEFT JOIN fileproj AS fmb
      ON fmb.fid = fgm.global_fid
  WHERE fmb.fid IS NULL;
