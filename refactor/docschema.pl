#!/usr/bin/perl
#
# Document the database schema
#
# $Id: docschema.pl,v 1.1 2005/05/16 11:35:26 dds Exp $
#

print '<?xml version="1.0" ?>
<notes>
<!-- Automatically generated file: $Id: docschema.pl,v 1.1 2005/05/16 11:35:26 dds Exp $ -->
The following sections describe the
schema of the database created through the SQL backend.
';
while (<>) {
	if (/BEGIN AUTOSCHEMA/../END AUTOSCHEMA/) {
		if (/CREATE TABLE (\w+)\(/) {
			print qq{
<h2>Table $1</h2>
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
			$type = 'CHARACTER VARYING' if (/varchar/);
			print "
<tr><td>$name</td><td>$type</td><td>$description</td></tr>
";
		} elsif (/\"\)\;\\n\"/) {
			print "</table>\n";
		}
	}
}

print "
</notes>
";
