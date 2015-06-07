#!/usr/bin/perl
#
# (C) Copyright 2001-2015 Diomidis Spinellis.
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
# Document the database schema
#
#

sub
printline
{
	return if ($name eq 'INTERNAL');
	$description =~ s/references (\w+)/references <a href="#$1">$1<\/a>/;
	print "
<tr><td>$name</td><td>$type</td><td>$description</td></tr>
";
}

$dir = $ARGV[0];
$dir =~ s,\/[^/]+$,/,;

print '<?xml version="1.0" ?>
<notes>
The following sections describe the
schema of the database created through the SQL backend.
';
while (<>) {
	if (/BEGIN AUTOSCHEMA/../END AUTOSCHEMA/) {
		if (/CREATE TABLE (\w+)\(.*\/\/ (.*)/) {
			print qq{
<a name="$1"> </a>
<h2>Table $1</h2>
<p>
$2.
</p>
<table border = "1">
<tr>
<th>Field name</th>
<th>Field type</th>
<th>Value description</th>
</tr>
			};
		} elsif (/^\s*\"(\w+).*\/\/ (.*)/) {
			$name = $1;
			$description = $2;
			$type = 'INTEGER' if (/INTEGER/);
			$type = 'BOOLEAN' if (/booltype/);
			$type = 'INTEGER or BIGINT<sup><a href="#note1">1</a></sup>' if (/ptrtype/);
			$type = 'CHARACTER VARYING' if (/varchar/);
			printline();
		} elsif (/AUTOSCHEMA INCLUDE ([^ ]+) (\w+)/) {
			$file = "$dir/$1";
			$part = $2;
			open(IN, $file) || die "Unable to open $file: $!\n";
			while (<IN>) {
				if (/BEGIN AUTOSCHEMA $part$/ .. /END AUTOSCHEMA $part$/) {
					if (/\{\s*\w+\,\s*\"(\w+)\"\,\s*\"([^"]+)\"/) {
						$name = $1;
						$description = $2;
						$type = (/\/\/ REAL$/ ? 'REAL' : 'INTEGER');
						printline();
					}
				}
			}
		} elsif (/\"\)\;\\n\"/) {
			print "</table>\n";
		}
	}
}

print '
<p>
<a name="note1">Note 1</a>: INTEGER on 32-bit architectures, BIGINT on 64-bit archiectures.
</notes>
';
