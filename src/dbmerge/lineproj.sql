-- Add to lineproj missing records

INSERT INTO lineproj
  SELECT
    fgm.global_fid AS fid,
    lpa.pid,
    lpa.lnum
  FROM adb.lineproj AS lpa
    LEFT JOIN fileid_to_global_map AS fgm
      ON fgm.dbid = 5 AND fgm.fid = lpa.fid
    LEFT JOIN lineproj AS lp
      ON lp.fid = fgm.global_fid AND lp.lnum = lpa.lnum
  WHERE lp.fid IS NULL;
