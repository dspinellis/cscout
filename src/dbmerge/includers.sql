-- Insert into the global includers table suitably mapped new entries

INSERT INTO includers
  SELECT 
    Coalesce(includers.pid, aincluders.pid) AS pid,
    cuid_map.global_fid AS cuid,
    basefileid_map.global_fid AS basefileid,
    includerid_map.global_fid AS includerid
    FROM adb.includers AS aincluders
    LEFT JOIN fileid_to_global_map AS cuid_map
      ON cuid_map.dbid = 5 AND cuid_map.fid = aincluders.cuid
    LEFT JOIN fileid_to_global_map AS basefileid_map
      ON basefileid_map.dbid = 5 AND basefileid_map.fid = aincluders.basefileid
    LEFT JOIN fileid_to_global_map AS includerid_map
      ON includerid_map.dbid = 5 AND includerid_map.fid = aincluders.includerid
    LEFT JOIN includers
      ON cuid_map.global_fid = includers.cuid
      AND basefileid_map.global_fid = includers.basefileid
      AND includerid_map.global_fid = includers.includerid
    WHERE includers.basefileid IS NULL;
