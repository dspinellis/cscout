-- Add missing paths to the files table, assigning them globally unique ids,
-- based on the already established fileid_to_global_map.

INSERT INTO files
  SELECT fileid_to_global_map.global_fid AS fid,
      afiles.name, afiles.ro
    FROM adb.files AS afiles
    LEFT JOIN fileid_to_global_map ON
      fileid_to_global_map.dbid = 5
      AND fileid_to_global_map.fid = afiles.fid
    LEFT JOIN files ON
      files.fid = fileid_to_global_map.global_fid
    WHERE files.name IS NULL;
