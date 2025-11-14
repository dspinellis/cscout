-- Insert into the global providers table suitably mapped new entries

INSERT INTO providers
  SELECT 
    Coalesce(providers.pid, aproviders.pid) AS pid,
    cuid_map.global_fid AS cuid,
    providerid_map.global_fid AS providerid
    FROM adb.providers AS aproviders
    INNER JOIN fileid_to_global_map AS cuid_map
      ON cuid_map.dbid = 5 AND cuid_map.fid = aproviders.cuid
    INNER JOIN fileid_to_global_map AS providerid_map
      ON providerid_map.dbid = 5 AND providerid_map.fid = aproviders.providerid
    LEFT JOIN providers
      ON cuid_map.global_fid = providers.cuid
      AND providerid_map.global_fid = providers.providerid
    WHERE providers.providerid IS NULL;
