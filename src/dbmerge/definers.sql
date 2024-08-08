-- Insert into the global definers table suitably mapped new entries

INSERT INTO definers
  SELECT 
    Coalesce(definers.pid, adefiners.pid) AS pid,
    cuid_map.global_fid AS cuid,
    basefileid_map.global_fid AS basefileid,
    definerid_map.global_fid AS definerid
    FROM adb.definers AS adefiners
    LEFT JOIN fileid_to_global_map AS cuid_map
      ON cuid_map.dbid = 5 AND cuid_map.fid = adefiners.cuid
    LEFT JOIN fileid_to_global_map AS basefileid_map
      ON basefileid_map.dbid = 5 AND basefileid_map.fid = adefiners.basefileid
    LEFT JOIN fileid_to_global_map AS definerid_map
      ON definerid_map.dbid = 5 AND definerid_map.fid = adefiners.definerid
    LEFT JOIN definers
      ON cuid_map.global_fid = definers.cuid
      AND basefileid_map.global_fid = definers.basefileid
      AND definerid_map.global_fid = definers.definerid
    WHERE definers.basefileid IS NULL;
