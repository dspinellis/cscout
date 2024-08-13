-- Insert into the global tokens table suitably mapped new entries

CREATE INDEX IF NOT EXISTS adb.idx_tokens_eid ON tokens(eid);
CREATE INDEX IF NOT EXISTS adb.idx_tokens_foffset ON tokens(foffset);

INSERT INTO tokens
  SELECT 
    fid_map.global_fid AS fid,
    atokens.foffset AS foffset,
    eid_map.global_eid AS eid
    FROM adb.tokens AS atokens
    LEFT JOIN fileid_to_global_map as fid_map
      ON fid_map.dbid = 5 AND fid_map.fid = atokens.fid
    LEFT JOIN eid_to_global_map AS eid_map
      ON eid_map.dbid = 5 AND eid_map.eid = atokens.eid
    LEFT JOIN tokens ON tokens.fid = fid_map.global_fid
      AND tokens.foffset = atokens.foffset
    WHERE tokens.eid IS NULL;
