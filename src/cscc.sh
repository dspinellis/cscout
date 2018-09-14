#!/bin/sh
#
# (C) Copyright 2006-2018 Diomidis Spinellis
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
# Process the specified C files like a compiler
#

ARGS="$@"

usage()
{
  echo "Usage: $0 [-E] [-s] file ..." 1>&2
  exit 1
}

PROCESS_CMD='#pragma process'
INCS=host-incs.h
DEFS=host-defs.h

while getopts d:Eg flag ; do
  case $flag in
  d)
    INST_DIR="$OPTARG"
    ;;
  E)
    PROCESS_CMD='#include'
    CSCOUT_FLAGS=-E
    ;;
  g)
    INCS=stdc-incs.h
    DEFS=stdc-defs.h
    ;;
  ?)
    usage
    ;;
  esac
done
shift $(($OPTIND - 1));

# Find and set the installation directory
if [ -n "$INST_DIR" ] ; then
  true
elif [ -d .cscout ] ; then
  INST_DIR=.cscout
elif [ -n "$CSCOUT_HOME" ] ; then
  INST_DIR="$CSCOUT_HOME"
elif [ -n "$HOME" -a -d "$HOME/.cscout" ] ; then
  INST_DIR="$HOME/.cscout"
elif [ -d 'INSTALL_INCLUDE' ] ; then
  INST_DIR='INSTALL_INCLUDE'
else
  cat 1>&2 <<\EOF
Unable to identify a CScout installation directory
Create ./.cscout, or create $HOME/.cscout, or set the $CSCOUT_HOME variable,
or provide it with the -d flag.
EOF
  exit 1
fi

# Setup temporary file in directory
CSFILE=".cscc-$$.cs"
trap 'rm -f "$CSFILE"' 0	# Remove it when exiting
trap 'exit 2' 1 2 15		# Exit when the program is interrupted

cat >$CSFILE <<EOF
/*
 *
 * CScout execution script
 * Automatically generated file from cscc.sh $ARGS
 *
 */

#pragma project "cscc"
#pragma block_enter

EOF

for f ; do
  cat >>$CSFILE <<EOF
#pragma echo "Processing $f\\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_include
#include "$INST_DIR/$INCS"
#include "$INST_DIR/$DEFS"
$PROCESS_CMD "$f"
#pragma echo "Done processing $f\\n"
#pragma block_exit

EOF
done

cat >>$CSFILE <<\EOF

#pragma block_exit
#pragma echo "Done processing project\n"
EOF

cscout $CSCOUT_FLAGS $CSFILE
