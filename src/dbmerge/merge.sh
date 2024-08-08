#!/bin/bash
#
# Merge the 32 generated database files into one
#

set -eu

DBID_FILE="dbid.txt"
TOOL_DIR=$(dirname $0)

# Get dbids from 33 onward. 1-32 are the databases to merge.
echo 33 >$DBID_FILE

:>log

# Log the specified message with a timestamp
log()
{
  echo "$(date -Is) $1" >>log
}

# Create an empty database onto which to consolidate the parts
create_empty()
{
  local name="$1"
  log "Create empty $name"
  rm -f "$name"
  sqlite3 file-0001.db .schema | sqlite3 "$name"
  cat <<\EOF | sqlite3 "$name"
DROP TABLE ids;

CREATE INDEX IF NOT EXISTS idx_files_name ON files(name);
CREATE INDEX idx_filemetrics_composite ON filemetrics(fid, precpp);
CREATE INDEX idx_definers_composite ON definers(cuid, basefileid, definerid);
CREATE INDEX idx_includers_composite ON includers(cuid, basefileid, includerid);
CREATE INDEX idx_inctriggers_composite ON inctriggers(cuid, basefileid, definerid, foffset, len);
CREATE INDEX IF NOT EXISTS idx_functions_composite ON functions(fid, foffset);

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

  log "BEGIN merge onto $dest $source"

  # Obtain the unique identifier for the database being merged
  local dbid=$(echo $source | awk -F'[-.]' '{print $2}')

  (
    echo "ATTACH DATABASE '$source' AS adb;"
    cd "$TOOL_DIR"
    cat \
      fileid_to_global_map.sql \
      files.sql \
      filemetrics.sql \
      definers.sql \
      includers.sql \
      providers.sql \
      inctriggers.sql \
      functionid_to_global_map.sql \
      functions.sql \
      functiondefs.sql \
      functionmetrics.sql \
      fcalls.sql
  ) |
    # Replace hard-coded database id 5 used for testing
    sed "s/\\<5\\>/$dbid/" |
    sqlite3 "$dest"
  log "END merge onto $dest $source"
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

files=($(seq 1 32 | xargs -n 1 printf 'file-%04d.db '))

result=$(merge "${files[@]}")
log "Finished merging into $result"

sqlite3 "$result" 'VACUUM;'
log "Finished vacuuming $result"

echo "Result in: $result"
