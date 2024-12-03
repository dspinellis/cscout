-- Speed up processing

CREATE INDEX IF NOT EXISTS tokens_eid_idx on tokens(eid);
CREATE INDEX IF NOT EXISTS tokens_fid_idx on tokens(fid);
