#!/bin/sh
#
# (C) Copyright 2024 Diomidis Spinellis
#
# This file is part of CScout.
#
# CScout is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CScout is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CScout.  If not, see <http://www.gnu.org/licenses/>.
#
# Cut out the specified file's processing from a CScout configuration
# file
#

# Fail on command errors and unset variables
set -eu

# Display usage information and exit
usage()
{
  cat <<EOF 1>&2
Usage: $(basename $0) [-e] (-F file-list|-f file|-p project) cscout-file

  -e            Output from specified point to the end
  -F file-list  Process only the specified files contained in file-list file
  -f file       Process only the specified file
  -p project    Process only the specified project
EOF
  exit 1
}

# File to cut
file=''

# File containing list of files to cut
file_list=''

# Non-empty if a project is being cut
project=''

# Non-empty if files until the end are to be cut
to_end=''


# Process command-line arguments
while getopts "eF:f:p:" opt; do
  case $opt in
    f)
      search="$OPTARG"
      file=1
      ;;
    F)
      file_list="$OPTARG"
      ;;
    e)
      to_end=1
      ;;
    p)
      search="project $OPTARG"
      project=1
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

# Ensure CScout file was passed as an argument
if [ $# -eq 0 ] ; then
  usage
fi

# CScout project file
csfile="$1"

# Extract the specified search pattern
extract()
{
  local search="$1"

  local pattern=$(echo "$search" | sed 's/\([].\\*$^\/[]\)/\\\1/g')

  local begin="/#pragma echo \"Processing $pattern\\\\n\"/"

  if [ -n "$to_end" ]; then
    local end=\$
  else
    local end="/#pragma echo \"Done processing $pattern\\\\n/"
  fi

  sed -n "${begin},${end}p" "$csfile"
}

if [ -z "$project" ] ; then
  cat <<\EOF
#pragma project "cscut"
#pragma block_enter
EOF
fi

if [ -n "$file_list" ] ; then
  while read search ; do
    extract "$search"
  done < "$file_list"
else
  extract "$search"
fi

if [ -z "$project" -a -z "$to_end" ] ; then
  echo '#pragma block_exit'
fi
