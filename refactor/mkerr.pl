#
# Generate the warning and error message list
#
# $Id: mkerr.pl,v 1.3 2003/06/21 16:18:28 dds Exp $
#


push(@ARGV, <*.cpp>);
push(@ARGV, <*.h>);
while (<>) {
	if (s/\@error//../\*\//) {
		s/\/\*//;
		s/\*\///;
		s/^\s*\* //;
		$expl .= $_;
	} elsif (/\:\:error\((\w+)\,\s*(.*)\);/ && defined($expl)) {
		$sev = $1;
		$msg = $2;
		$msg =~ s/\\\"//g;
		$msg =~ s/^string\(//;
		$msg =~ s/\"//;
		$msg =~ s/\"[^"]*\"/ ... /g;
		$msg =~ s/\"$//;
		$msg =~ s/\".+$/ .../;
		$msg =~ s//"/g;
		substr($msg, 0, 1) = uc(substr($msg, 0, 1));
		$expl =~ s/\s+$//;
		$expl .= '.' unless ($expl =~ m/\.$/);
		if (!$seen{$msg . $expl}) {
			push(@{$error{$sev}},
				'<li> <code>' . $msg . '</code><br>' . $expl . '<p>');
			$seen{$msg . $expl} = 1;
		}
		undef $expl;
	}
}

%title = (
	E_WARN => 'Warnings',
	E_FATAL => 'Fatal Errors',
	E_ERR => 'Errors',
);

print '<?xml version="1.0" ?>
<notes>
';

for $sev (keys %error) {
	print "<h2>$title{$sev}</h2>\n<ul>\n";
	print sort @{$error{$sev}};
	print "</ul>\n";
}

print "\n</notes>\n";
