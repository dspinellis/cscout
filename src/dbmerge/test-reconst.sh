#!/bin/sh


set -eu

# Display usage information and exit
usage()
{
  local message="$1"

  cat <<EOF 1>&2
$message
Usage: $(basename $0) -t [OPTION]... database.db
       $(basename $0) -f file [OPTION]... database.db

Test the reconstitution of files in a CScout database

  -t        Test the reconstitution of all files.
            Related options:
  -k        Keep comparing after finding a difference.
  -q        Run a quick diff; do not list the full diff output.
  -s        Provide summary of each file's result.

  -f file   Reconstitute the specified file.
            Available options:
  -b line   Beginning line number.
  -e line   Ending line number.
  -o        Output offset values.

Example:
  test-reconst -t result.db
  test-reconst -f file.c result.db
EOF
  exit 1
}

opt_test=''
opt_keep_going=''
opt_quick=''
opt_summary=''

opt_file=''
opt_begin='0'
opt_end='99999999'
opt_offset=''

# Process command-line arguments
while getopts "tkqsf:b:e:o" opt; do
  case $opt in
    b)
      opt_begin="(SELECT foffset FROM linepos CROSS JOIN selected_file AS sf
        WHERE fid=sf.value AND lnum <= $OPTARG LIMIT 1)"
      ;;
    e)
      opt_end="(SELECT foffset FROM linepos CROSS JOIN selected_file AS sf
        WHERE fid=sf.value AND lnum >= $OPTARG LIMIT 1)"
      ;;
    f)
      opt_file="$OPTARG"
      ;;
    k)
      opt_keep_going=1
      ;;
    o)
      opt_offset='foffset, '
      ;;
    q)
      opt_quick=-q
      ;;
    s)
      opt_summary=1
      ;;
    t)
      opt_test=1
      ;;
    \?) # Illegal option
      usage ''
      ;;
    :)
      usage "Option -$OPTARG requires an argument."
      ;;
  esac
done

shift "$((OPTIND-1))"

# Database must be provided
test $# -ne 1 && usage "Database name not specified"
DB="$1"

# Both cannot be set
test -n "$opt_file" -a -n "$opt_test" && usage "-t and -f cannot both be set"

# At least one must be set
test -z "$opt_file" -a -z "$opt_test" && usage "At least one of -t or -f must be set"


# Run diff on the specified path with output controlled by opt_quick
run_diff()
{
  local path="$1"

  if [ -n "$opt_quick" ] ; then
    diff $opt_quick -w $path try.c >/dev/null
  else
    diff $opt_quick -w $path try.c
  fi
}

# Output the selection query for all elements of the specified file
all_elements()
{
  local fid="$1"

cat <<EOF
  SELECT foffset, name AS s
    FROM ids
    INNER JOIN tokens USING(eid)
    WHERE fid = $fid
  UNION
  SELECT foffset, code AS s
    FROM rest
    WHERE fid = $fid
  UNION
  SELECT foffset, comment AS s
    FROM comments
    WHERE fid = $fid
  UNION
  SELECT foffset, string AS s
    FROM strings
    WHERE fid = $fid
EOF
}

# Fix newlines
fix_nl()
{
  tr -d "\n\r" |
  perl -pe 's/\\u0000d\\u0000a/\n/g;s/\\u0000a/\n/g'
}

# Test reconstitution of all files
test_all()
{
  sqlite3 "$DB" 'select distinct fid, name from files;' |
  while IFS='|' read fid path ; do
    echo "SELECT s FROM ( $(all_elements $fid) ) ORDER BY foffset;" |
tee query.sql |
    sqlite3 "$DB" |
      fix_nl >try.c

    if run_diff "$path" ; then
      test -n "$opt_summary" && echo ✓ $path
    else
      result=$?
      test -n "$opt_summary" && echo ✗ $path
      test $result -a -z "$opt_keep_going" && exit 1
    fi
  done
}

# List the contents of a single file
file_list()
{
  cat <<EOF |
WITH selected_file AS (
  SELECT fid AS value FROM files WHERE name='$opt_file'
),
elements AS (
  $(all_elements '(SELECT value from selected_file)')
)
SELECT $opt_offset s
  FROM elements
  WHERE foffset BETWEEN
    $opt_begin
  AND
    $opt_end
  ORDER BY foffset;
EOF
tee query.sql |
  sqlite3 "$DB" |
  if [ -n "$opt_offset" ] ; then
    cat
  else
    fix_nl
  fi
}

test -n "$opt_test" && test_all
test -n "$opt_file" && file_list "$opt_file"

