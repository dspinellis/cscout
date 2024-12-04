-- Speed up processing

CREATE INDEX IF NOT EXISTS idx_tokens_eid ON tokens(eid);
CREATE INDEX IF NOT EXISTS idx_tokens_fid_idx on tokens(fid);

CREATE INDEX IF NOT EXISTS idx_fcalls_destid ON fcalls(destid);
CREATE INDEX IF NOT EXISTS idx_fcalls_sourceid ON fcalls(sourceid);

CREATE INDEX IF NOT EXISTS idx_functiondefs_functionid ON functiondefs(functionid);

CREATE INDEX IF NOT EXISTS idx_ids_name ON ids(name);
CREATE INDEX IF NOT EXISTS idx_ids_name ON ids(eid);

CREATE INDEX IF NOT EXISTS idx_includers_cuid ON includers(cuid);
CREATE INDEX IF NOT EXISTS idx_includers_includerid ON includers(includerid);
