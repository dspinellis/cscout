#!/bin/sh
#
# (C) Copyright 2016 Diomidis Spinellis
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
# Install the include files required for CScout
#

PREFIX="${1-/usr/local/include}"
TMPFILE="/tmp/$0-$$"
INC=../include
TARGET="$PREFIX/cscout"
MODE=644

# Generic C definitions
install -m $MODE -D -t "$TARGET/stdc" $INC/stdc/*.h
cat $INC/template/cscout-defs.h $INC/template/stdc-defs.h >$TMPFILE
install -m $MODE $TMPFILE "$TARGET/stdc-defs.h"

# Generic C include path specification
cat <<EOF >$TMPFILE
#pragma includepath "$TARGET/stdc"

/* Avoid unused include file warnings */
static void _cscout_dummy2(void) { _cscout_dummy2(); }
EOF
install -m $MODE $TMPFILE "$TARGET/stdc-incs.h"

# Host's C definitions
{
  cat $INC/template/cscout-defs.h $INC/template/stdc-defs.h
  echo "/* Definitions derived from cpp -O -dM on $(date) */"
  cpp -O -dM /dev/null | sort
} >$TMPFILE
install -m $MODE $TMPFILE "$TARGET/host-defs.h"

# Host's C include path specification
cpp -Wp,-v </dev/null 2>&1 |
sed -n -e '
1a\
/* Avoid unused include file warnings */
1a\
static void _cscout_dummy2(void) { _cscout_dummy2(); }
1a\

/^#include <\.\.\.> search starts here:/,/^End of search list\./ {
 /^ /!d
 s/ /#pragma includepath "/
 s/$/"/
 p
}' >$TMPFILE
install -m $MODE $TMPFILE "$TARGET/host-incs.h"
