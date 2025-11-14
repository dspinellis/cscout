-- Insert into the global fcalls table suitably mapped new entries

CREATE TABLE new_fcalls(
  sourceid BIGINT, -- Calling function identifier key
  destid BIGINT -- Called function identifier key
);

INSERT INTO new_fcalls
  SELECT source_map.global_id AS sourceid,
    dest_map.global_id AS destid
    FROM adb.fcalls AS afcalls
    INNER JOIN functionid_to_global_map AS source_map
      ON source_map.dbid = 5
        AND source_map.id = afcalls.sourceid
    INNER JOIN functionid_to_global_map AS dest_map
      ON dest_map.dbid = 5
        AND dest_map.id = afcalls.destid
  UNION
  SELECT source_map.global_id AS sourceid,
    dest_map.global_id AS destid
    FROM fcalls AS ofcalls
    INNER JOIN functionid_to_global_map AS source_map
      ON source_map.dbid != 5
        AND source_map.id = ofcalls.sourceid
    INNER JOIN functionid_to_global_map AS dest_map
      ON dest_map.dbid != 5
        AND dest_map.id = ofcalls.destid;

DROP TABLE fcalls;
ALTER TABLE new_fcalls RENAME TO fcalls;
