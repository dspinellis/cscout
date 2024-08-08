-- Insert into the global functiondefs table suitably mapped new entries

INSERT INTO functiondefs
  SELECT functionid_to_global_map.global_id AS id,
      begin_map.global_fid AS fidbegin,
      afunctiondefs.foffsetbegin as foffsetbegin,
      end_map.global_fid AS fidend,
      afunctiondefs.foffsetend as foffsetend
    FROM adb.functiondefs AS afunctiondefs
    LEFT JOIN fileid_to_global_map AS begin_map
      ON begin_map.dbid = 5 AND begin_map.fid = afunctiondefs.fidbegin
    LEFT JOIN fileid_to_global_map AS end_map
      ON end_map.dbid = 5 AND end_map.fid = afunctiondefs.fidend
    LEFT JOIN functionid_to_global_map
      ON functionid_to_global_map.dbid = 5
        AND functionid_to_global_map.id = afunctiondefs.functionid
    LEFT JOIN functiondefs
      ON functiondefs.functionid = functionid_to_global_map.global_id
    WHERE functiondefs.functionid IS NULL;
