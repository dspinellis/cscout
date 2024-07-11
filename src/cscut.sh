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
Usage: $(basename $0) [-e] (-p project|-f file) cscout-file

  -e         Output from specified point to the end
  -f file    Process only the specified file
  -p project Process only the specified project
EOF
  exit 1
}

FILE=''
TO_END=''

# Process command-line arguments
while getopts "ef:p:" opt; do
  case $opt in
    f)
      search="$OPTARG"
      FILE=1
      ;;
    e)
      TO_END=1
      ;;
    p)
      search="project $OPTARG"
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

pattern=$(echo "$search" | sed 's/\([].\\*$^\/[]\)/\\\1/g')

begin="/#pragma echo \"Processing $pattern\\\\n\"/"

if [ -n "$TO_END" ]; then
  end=\$
else
  end="/#pragma echo \"Done processing $pattern\\\\n/"
fi

if [ -n "$FILE" ] ; then
  cat <<\EOF
#pragma project "cscut"
#pragma block_enter
EOF
fi

sed -n "${begin},${end}p" "$1"

if [ -n "$FILE" ] ; then
  echo '#pragma block_exit'
fi
