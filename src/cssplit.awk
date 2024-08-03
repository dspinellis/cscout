#!/usr/bin/env awk -f
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
# Split CScout processing input into files containing the specified number
# of compilation units (CUs).
#

function begin_project() {
	outfile = sprintf("file-%04d.cs", ++file_number)
	print "#pragma project \"cssplit\"" >outfile
	print "#pragma block_enter" >outfile
}

function end_project() {
	print "#pragma block_exit" >outfile
	close(outfile)
}

BEGIN {
	if (ARGC < 2) {
		print "Usage: cssplit N" > "/dev/stderr"
		exit 1
	}

	N = ARGV[1]
	ARGV[1] = ""
}

# CU processing path name starts with a /
/#pragma echo "Processing \// {
	if (cu_count % N == 0) {
		if (cu_count > 0)
			end_project();
		begin_project();
	}
	in_cu = 1;
	cu_count++
}

in_cu {
	print $0 >outfile
}

# CU processing path name starts with a /
/#pragma echo "Done processing \// {
	in_cu = 0
}

END {
	if (cu_count > 0)
		end_project();
}
