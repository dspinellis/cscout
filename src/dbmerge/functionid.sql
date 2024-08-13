-- Insert into the global functionid table suitably mapped new entries

INSERT INTO functionid
  SELECT fid_map.global_id AS functionid,
      afunctionid.ordinal AS ordinal,
      eid_map.global_eid AS eid
    FROM adb.functionid AS afunctionid
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid = 5 AND eid_map.eid = afunctionid.eid
    LEFT JOIN functionid_to_global_map AS fid_map
      ON fid_map.dbid = 5 AND fid_map.id = afunctionid.functionid
    LEFT JOIN functionid
      ON functionid.functionid = fid_map.global_id
    WHERE functionid.functionid IS NULL;
