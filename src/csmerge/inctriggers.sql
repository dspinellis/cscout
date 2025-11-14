-- Insert into the global inctriggers table suitably mapped new entries

INSERT INTO inctriggers
  SELECT 
    Coalesce(inctriggers.pid, ainctriggers.pid) AS pid,
    cuid_map.global_fid AS cuid,
    basefileid_map.global_fid AS basefileid,
    definerid_map.global_fid AS definerid,
    ainctriggers.foffset AS foffset,
    ainctriggers.len AS len
    FROM adb.inctriggers AS ainctriggers
    INNER JOIN fileid_to_global_map AS cuid_map
      ON cuid_map.dbid = 5 AND cuid_map.fid = ainctriggers.cuid
    INNER JOIN fileid_to_global_map AS basefileid_map
      ON basefileid_map.dbid = 5 AND basefileid_map.fid = ainctriggers.basefileid
    INNER JOIN fileid_to_global_map AS definerid_map
      ON definerid_map.dbid = 5 AND definerid_map.fid = ainctriggers.definerid
    LEFT JOIN inctriggers
      ON cuid_map.global_fid = inctriggers.cuid
      AND basefileid_map.global_fid = inctriggers.basefileid
      AND definerid_map.global_fid = inctriggers.definerid
      AND ainctriggers.foffset = inctriggers.foffset
      AND ainctriggers.len = inctriggers.len
    WHERE inctriggers.basefileid IS NULL;
