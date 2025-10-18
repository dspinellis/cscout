#!/bin/bash
#
# Merge the generated database files into one
#

set -eu
set -o pipefail

# Limit the damage of a fault leading to a fork bomb
ulimit -u 500

DBID_FILE="dbid.txt"
TOOL_DIR=$(dirname $0)
LOG_FILE=dbmerge.log

if [ $# -ne 1 ] ; then
  echo "Usage: $(basename $0) nfiles" 1>&2
  exit 1
fi

NFILES="$1"

# Get dbids from N+1 onward. 1-N are the databases to merge.
echo $((NFILES + 1)) >$DBID_FILE

:>$LOG_FILE

# Log the specified message with a timestamp
log()
{
  echo "$(date -Is) $1" >>$LOG_FILE
}

# Create an empty database onto which to consolidate the parts
create_empty()
{
  local name="$1"
  log "Create empty $name"
  rm -f "$name"
  sqlite3 file-0001.db .schema | sqlite3 "$name"
  cat <<\EOF | sqlite3 "$name"
CREATE INDEX idx_definers_composite ON definers(cuid, basefileid, definerid);
CREATE INDEX idx_filemetrics_composite ON filemetrics(fid, precpp);
CREATE INDEX idx_files_name ON files(name);
CREATE INDEX idx_functions_id ON functions(id);
CREATE INDEX idx_functionid_eid ON functionid(eid);
CREATE INDEX idx_includers_composite ON includers(cuid, basefileid, includerid);
CREATE INDEX idx_inctriggers_composite ON inctriggers(cuid, basefileid, definerid, foffset, len);
CREATE INDEX idx_tokens_eid ON tokens(eid);
CREATE INDEX idx_tokens_foffset ON tokens(foffset);


CREATE TABLE fileid_to_global_map(
  dbid INTEGER,         -- Unmerged database identifier
  fid INTEGER,          -- File identifier in an unmerged database
  global_fid INTEGER,    -- Corresponding fid used across all databases
  PRIMARY KEY(dbid, fid)
);
CREATE INDEX idx_fileid_to_global_map ON fileid_to_global_map(global_fid);

CREATE TABLE functionid_to_global_map(
  dbid INTEGER,         -- Unmerged database identifier
  id INTEGER,           -- Function identifier in an unmerged database
  global_id INTEGER,     -- Corresponding id used across all databases
  PRIMARY KEY(dbid, id)
);
CREATE INDEX idx_functionid_to_global_map ON functionid_to_global_map(global_id);
EOF
}

# Obtain a new database id by atomically incrementing the corresponding counter
get_dbid()
{
    (
        # Open the counter file in read/write mode and obtain a lock
        flock -x 200

        counter=$(<"$DBID_FILE")
        counter=$((counter + 1))
        echo $counter | tee "$DBID_FILE"
    ) 200<> "$DBID_FILE"
}

merge_onto()
{
  local dest="$1"
  local source="$2"

  # Obtain the unique identifier for the database being merged
  local dbid=$(echo $source | awk -F'[-.]' '{print $2 + 0}')

  log "DB $dbid: BEGIN merge onto $dest $source"

  # Order matters here
  # eclasses populates: tokens, ids, functionid, functionid_to_global_map,
  # idproj.
  # We assume that projects are defined with the same names and order
  # in all shards.  This is the way csshard works.  So there's nothing to do
  # for the projects table.
  for i in \
    fileid_to_global_map.sql \
    files.sql \
    eclasses.sql \
    filemetrics.sql \
    definers.sql \
    includers.sql \
    providers.sql \
    inctriggers.sql \
    functions.sql \
    functiondefs.sql \
    functionmetrics.sql \
    fcalls.sql \
    fileproj.sql \
    lineproj.sql \
    comments.sql \
    strings.sql \
    rest.sql \
    linepos.sql
   do
     set -eu
     set -o pipefail
     {
       log "DB $dbid: running $i"
       # Exit on errors
       echo ".bail on"
       # Time issued commands
       echo ".timer on"
       echo "ATTACH DATABASE '$source' AS adb;"
       # Replace hard-coded database id 5 used for testing
       sed "s/\\<5\\>/$dbid/g" "$TOOL_DIR/$i"
     } |
     sqlite3 "$dest" 2>&1 |
     while read -r line ; do log "DB $dbid: $line" ; done
   done
  log "DB $dbid: END merge onto $dest $source"
}

merge()
{
  local files=("$@")

  if [ "${#files[@]}" -eq 2 ]; then
    output="temp-$(get_dbid).db"
    create_empty "$output"
    merge_onto $output "${files[0]}"
    merge_onto $output "${files[1]}"
    echo $output
    return
  fi

  midpoint=$((${#files[@]} / 2))

  local left=("${files[@]:0:midpoint}")
  local left_output=$(mktemp XXXXX.txt)
  merge "${left[@]}" >$left_output &
  pid_left=$!

  local right=("${files[@]:midpoint}")
  local right_output=$(mktemp XXXXX.txt)
  merge "${right[@]}" >$right_output &
  pid_right=$!

  wait $pid_left
  left_output_db=$(<$left_output)
  rm $left_output

  wait $pid_right
  right_output_db=$(<$right_output)
  rm $right_output

  merge_onto $left_output_db $right_output_db
  echo $left_output_db
}

files=($(seq 1 $NFILES | xargs -n 1 printf 'file-%04d.db '))

result=$(merge "${files[@]}")
log "Finished merging into $result"

cat <<\EOF | sqlite3 "$result"
ALTER TABLE filemetrics ADD COLUMN iscscout BOOLEAN;
UPDATE filemetrics SET iscscout = (
  SELECT (name LIKE '%.cs')
    FROM files
    WHERE filemetrics.fid = files.fid
);
CREATE TABLE cscout_files AS
  SELECT fid FROM files WHERE name LIKE '%.cs' OR name LIKE '%/csmake-%-defs.h';
VACUUM;
ANALYZE;
EOF
log "Finished completing and vacuuming $result"

rm -f merged.db
ln "$result" merged.db
echo "Result in: merged.db"
