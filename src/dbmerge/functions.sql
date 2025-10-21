-- Create global functions table with suitably mapped entries

CREATE TABLE new_functions(
  ID BIGINT PRIMARY KEY, -- Unique function identifier
  NAME CHARACTER VARYING, -- Function name (redundant; see FUNCTIONID)
  ISMACRO BOOLEAN, -- True if a function-like macro (otherwise a C function)
  DEFINED BOOLEAN, -- True if the function is defined within the workspace
  DECLARED BOOLEAN, -- True if the function is declared within the workspace
  FILESCOPED BOOLEAN, -- True if the function's scope is a single compilation unit (static or macro)
  FID INTEGER, -- File key of the function's definition, declaration, or use
  FOFFSET INTEGER, -- Offset of definition, declaration, or use within the file
  FANIN INTEGER, -- Fan-in (number of callers)
  FOREIGN KEY(FID) REFERENCES FILES(FID)
);

WITH fa AS (
  SELECT functionid_map.global_id AS id,
      name, ismacro, defined, declared, filescoped,
      fileid_map.global_fid AS fid, foffset, fanin
    FROM adb.functions AS f
    LEFT JOIN fileid_to_global_map AS fileid_map
      ON fileid_map.dbid = 5 AND fileid_map.fid = f.fid
    LEFT JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid = 5 AND functionid_map.id = f.id
),
fb AS (
  SELECT functionid_map.global_id AS id,
      name, ismacro, defined, declared, filescoped,
      fileid_map.global_fid AS fid, foffset, fanin
    FROM functions AS f
    LEFT JOIN fileid_to_global_map AS fileid_map
      ON fileid_map.dbid != 5 AND fileid_map.global_fid = f.fid
    LEFT JOIN functionid_to_global_map AS functionid_map
      ON functionid_map.dbid != 5 AND functionid_map.id = f.id
)
INSERT INTO new_functions
  SELECT
      Coalesce(fa.id, fb.id) AS id,
      Coalesce(fa.name, fb.name) AS name,
      Coalesce(fa.ismacro, fb.ismacro) OR Coalesce(fb.ismacro, fa.ismacro) AS ismacro,
      Coalesce(fa.defined, fb.defined) OR Coalesce(fb.defined, fa.defined) AS defined,
      Coalesce(fa.declared, fb.declared) OR Coalesce(fb.declared, fa.declared) AS declared,
      Coalesce(fa.filescoped, fb.filescoped) OR Coalesce(fb.filescoped, fa.filescoped) AS filescoped,
      Coalesce(fa.fid, fb.fid) AS fid,
      Coalesce(fa.foffset, fb.foffset) AS foffset,
      Max(Coalesce(fa.fanin, fb.fanin), Coalesce(fb.fanin, fa.fanin)) AS fanin
    FROM fa FULL OUTER JOIN fb ON fa.id = fb.id;

DROP TABLE functions;
ALTER TABLE new_functions RENAME TO functions;
