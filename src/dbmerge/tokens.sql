-- Create global tokens table with suitably mapped existing entries
-- The eid_to_global_map table remaps all ECs, so all records have to be
-- created anew.

CREATE INDEX IF NOT EXISTS adb.idx_tokens_eid ON tokens(eid);
CREATE INDEX IF NOT EXISTS adb.idx_tokens_foffset ON tokens(foffset);

CREATE TABLE new_tokens(
  fid INTEGER, -- File key
  foffset INTEGER, -- Offset within the file
  eid BIGINT, -- Identifier key
  PRIMARY KEY(fid, foffset),
  FOREIGN KEY(fid) REFERENCES files(fid),
  FOREIGN KEY(eid) REFERENCES ids(eid)
);

INSERT INTO new_tokens
  SELECT
      fid_map.global_fid AS fid,
      atokens.foffset,
      eid_map.global_eid AS eid
    FROM adb.tokens AS atokens
    LEFT JOIN fileid_to_global_map as fid_map
      ON fid_map.dbid = 5 AND fid_map.fid = atokens.fid
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid = 5 AND eid_map.eid = atokens.eid
  UNION
  SELECT
      tokens.fid,
      tokens.foffset,
      eid_map.global_eid AS eid
    FROM tokens
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid is null AND eid_map.eid = tokens.eid;

DROP TABLE IF EXISTS tokens;
ALTER TABLE new_tokens RENAME TO tokens;

CREATE INDEX IF NOT EXISTS idx_tokens_eid ON tokens(eid);
CREATE INDEX IF NOT EXISTS idx_tokens_foffset ON tokens(foffset);
