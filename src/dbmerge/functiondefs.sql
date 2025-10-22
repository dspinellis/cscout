-- Create global functiondefs table with suitably mapped entries

CREATE TABLE new_FUNCTIONDEFS(
  FUNCTIONID BIGINT PRIMARY KEY, -- Function identifier key
  FIDBEGIN INTEGER, -- File key of the function's definition begin
  FOFFSETBEGIN INTEGER, -- Offset of definition begin within the file
  FIDEND INTEGER, -- File key of the function's definition end
  FOFFSETEND INTEGER, -- Offset of definition end within the file
  FOREIGN KEY(FUNCTIONID) REFERENCES FUNCTIONS(ID)
);

INSERT INTO new_functiondefs
  SELECT functionid_map.global_id AS functionid,
      fileid_begin_map.global_fid AS fidbegin,
      foffsetbegin,
      fileid_end_map.global_fid AS fidend,
      foffsetend
    FROM adb.functiondefs AS f
    INNER JOIN fileid_to_global_map AS fileid_begin_map
      ON fileid_begin_map.dbid = 5 AND fileid_begin_map.fid = f.fidbegin
    INNER JOIN fileid_to_global_map AS fileid_end_map
      ON fileid_end_map.dbid = 5 AND fileid_end_map.fid = f.fidend
    INNER JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid = 5 AND functionid_map.id = f.functionid;

INSERT OR IGNORE INTO new_functiondefs
  SELECT functionid_map.global_id AS functionid,
      fileid_begin_map.global_fid AS fidbegin,
      foffsetbegin,
      fileid_end_map.global_fid AS fidend,
      foffsetend
    FROM functiondefs AS f
    INNER JOIN fileid_to_global_map AS fileid_begin_map
      ON fileid_begin_map.dbid != 5
        AND fileid_begin_map.global_fid = f.fidbegin
    INNER JOIN fileid_to_global_map AS fileid_end_map
      ON fileid_end_map.dbid != 5
        AND fileid_end_map.global_fid = f.fidend
    INNER JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid != 5 AND functionid_map.id = f.functionid;

DROP TABLE functiondefs;
ALTER TABLE new_functiondefs RENAME TO functiondefs;
