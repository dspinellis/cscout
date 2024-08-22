-- Create global functionid table with suitably mapped existing entries
-- The eid_to_global_map table remaps all ECs, so all records have to be
-- created anew.

CREATE TABLE new_functionid(
  functionid BIGINT, -- Function identifier key
  ordinal INTEGER, -- Position of the identifier within the function name (0-based)
  eid BIGINT, -- Identifier key
  PRIMARY KEY(functionid, ordinal),
  FOREIGN KEY(functionid) REFERENCES functions(id),
  FOREIGN KEY(eid) REFERENCES ids(eid)
);


INSERT INTO new_functionid
  SELECT fid_map.global_id AS functionid,
        afunctionid.ordinal AS ordinal,
        eid_map.global_eid AS eid
    FROM adb.functionid AS afunctionid
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid = 5 AND eid_map.eid = afunctionid.eid
    LEFT JOIN functionid_to_global_map AS fid_map
      ON fid_map.id = afunctionid.functionid
    LEFT JOIN functionid
      ON functionid.functionid = fid_map.global_id
    WHERE functionid.functionid IS NULL
  UNION
  SELECT fid_map.global_id AS functionid,
        functionid.ordinal AS ordinal,
        eid_map.global_eid AS eid
    FROM functionid
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid is null AND eid_map.eid = functionid.eid
    LEFT JOIN functionid_to_global_map AS fid_map
      ON fid_map.global_id = functionid.functionid
    LEFT JOIN adb.functionid AS afunctionid
      ON afunctionid.functionid = fid_map.id
    WHERE afunctionid.functionid IS NULL;

DROP TABLE IF EXISTS functionid;
ALTER TABLE new_functionid RENAME TO functionid;
