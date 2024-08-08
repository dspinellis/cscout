-- Insert into the global functions table suitably mapped new entries
-- setting id to a globally unique identifier.
-- This table can then be used for mapping fid/foffset into function id
-- for all the other tables containing functionid.

WITH max_id AS (
  SELECT IFNULL(MAX(id), 0) AS max_id FROM functions
)
INSERT INTO functions
  SELECT max_id.max_id + ROW_NUMBER() OVER () AS id,
      afunctions.name,
      afunctions.ismacro,
      afunctions.defined,
      afunctions.declared,
      afunctions.filescoped,
      fileid_to_global_map.global_fid AS fid,
      afunctions.foffset,
      afunctions.fanin
    FROM adb.functions AS afunctions
    LEFT JOIN fileid_to_global_map
      ON fileid_to_global_map.fid = afunctions.fid * 100 + 5
    LEFT JOIN functions
      ON functions.fid = fileid_to_global_map.global_fid
      AND functions.foffset = afunctions.foffset
    CROSS JOIN max_id
    WHERE functions.id IS NULL;
