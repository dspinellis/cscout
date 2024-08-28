-- Create empty functionid tables for tests where they aren't needed

CREATE TABLE FUNCTIONID(
  FUNCTIONID BIGINT,
  ORDINAL INTEGER,
  EID BIGINT
);

CREATE TABLE adb.FUNCTIONID(
  FUNCTIONID BIGINT,
  ORDINAL INTEGER,
  EID BIGINT
);

CREATE TABLE functionid_to_global_map(
  dbid INTEGER,         -- Unmerged database identifier
  id INTEGER,           -- Function identifier in an unmerged database
  global_id INTEGER,     -- Corresponding id used across all databases
  PRIMARY KEY(dbid, id)
);
