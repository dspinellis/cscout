#!/usr/bin/perl
#
# Process a PicoQL schema file replacing the #schema sections with
# the actual fields
#

while (<>) {
	if (/^(\s*)#schema (\S+)\s+(\w+)\s+(\w+)\s+(.*)/) {
		my $indent = $1;
		my $file = $2;
		my $schema = $3;
		my $type = $4;
		my $expr = $5;
		open(IN, $file) || die "Unable to open $file: $!\n";
		while (<IN>) {
			if (/BEGIN AUTOSCHEMA $schema$/ .. /END AUTOSCHEMA $schema$/) {
				if (/\{\s*(\w+)\,\s*\"(\w+)\"\,\s*\"([^"]+)\"/) {
					my $fieldname = $1;
					my $name = $2;
					next if ($name eq 'INTERNAL');
					my $description = $3;
					my $e = $expr;
					$e =~ s/FIELDNAME/$fieldname/g;
					print "$indent$name $type FROM $e,\n";
				}
			}
		}
	} else {
		print;
	}
}
