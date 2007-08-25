#!/usr/bin/perl
#
# Document the database schema
#
# $Id: docschema.pl,v 1.7 2007/08/25 05:29:29 dds Exp $
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
<!-- Automatically generated file: $Id: docschema.pl,v 1.7 2007/08/25 05:29:29 dds Exp $ -->
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
