-- Insert or update into the global functions table suitably mapped new entries

INSERT OR REPLACE INTO functions
  SELECT functionid_map.global_id AS id,
      Coalesce(fa.name, fb.name) AS name,
      Coalesce(fa.ismacro, fb.ismacro) OR Coalesce(fb.ismacro, fa.ismacro) AS ismacro,
      Coalesce(fa.defined, fb.defined) OR Coalesce(fb.defined, fa.defined) AS defined,
      Coalesce(fa.declared, fb.declared) OR Coalesce(fb.declared, fa.declared) AS declared,
      Coalesce(fa.filescoped, fb.filescoped) OR Coalesce(fb.filescoped, fa.filescoped) AS filescoped,
      fileid_map.global_fid AS fid,
      Coalesce(fa.foffset, fb.foffset) AS foffset,
      Max(Coalesce(fa.fanin, fb.fanin), Coalesce(fb.fanin, fa.fanin)) AS fanin
    FROM adb.functions AS fa
    INNER JOIN fileid_to_global_map AS fileid_map
      ON fileid_map.dbid = 5 AND fileid_map.fid = fa.fid
    INNER JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid = 5 AND functionid_map.id = fa.id
    LEFT JOIN functions AS fb
      ON fb.id = functionid_map.global_id;
