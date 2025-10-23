-- Insert into the global fcalls table suitably mapped new entries

INSERT INTO fcalls
  SELECT source_map.global_id AS sourceid,
    dest_map.global_id AS destid
    FROM adb.fcalls AS afcalls
    INNER JOIN functionid_to_global_map AS source_map
      ON source_map.dbid = 5
        AND source_map.id = afcalls.sourceid
    INNER JOIN functionid_to_global_map AS dest_map
      ON dest_map.dbid = 5
        AND dest_map.id = afcalls.destid
    LEFT JOIN fcalls
      ON fcalls.sourceid = source_map.global_id
        AND fcalls.destid = dest_map.global_id
    WHERE fcalls.sourceid IS NULL AND fcalls.destid IS NULL;
