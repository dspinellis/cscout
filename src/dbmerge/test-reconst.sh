#!/bin/sh


set -eu

# Display usage information and exit
usage()
{
  cat <<EOF 1>&2
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
opt_begin=''
opt_end=''

# Process command-line arguments
while getopts "tkqsfb:e:o" opt; do
  case $opt in
    k)
      opt_keep_going=1
      ;;
    q)
      opt_quick=-q
      ;;
    s)
      opt_summary=1
      ;;
    \?) # Illegal option
      usage
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      usage
      ;;
  esac
done

shift "$((OPTIND-1))"

if [ $# -ne 1 ] ; then
  usage
fi
DB="$1"

sqlite3 "$DB" 'select distinct fid, name from files;' |
while IFS='|' read fid path ; do
  # echo "Comparing $path" 1>&2
  cat <<EOF |
select s from
(select name as s, foffset  from ids inner join tokens on
ids.eid = tokens.eid where fid = $fid
union select code as s, foffset from rest where fid = $fid
union select comment as s, foffset from comments where fid = $fid
union select string as s, foffset from strings where fid = $fid
)
order by foffset;
EOF
  tee in.sql |
  sqlite3 "$DB" |
  tr -d "\n\r" |
  perl -pe 's/\\u0000d\\u0000a/\n/g;s/\\u0000a/\n/g' >try.c

run_diff()
{
  if [ -n "$opt_quick" ] ; then
    diff $opt_quick -w $path try.c >/dev/null
  else
    diff $opt_quick -w $path try.c
  fi
}

  if run_diff ; then
    test -n "$opt_summary" && echo ✓ $path
  else
    result=$?
    test -n "$opt_summary" && echo ✗ $path
    test $result -a -z "$opt_keep_going" && exit 1
  fi
done
