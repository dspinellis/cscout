#!/usr/bin/perl
#
# Spy on ar invocations and construct corresponding CScout directives
#
#
# (C) Copyright 2005 Diomidis Spinellis.  All rights reserved.
#

use Cwd 'abs_path';

$origline = "ar " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

@ARGV2 = @ARGV;
$op = shift @ARGV2;

if ($op !~ m/[rmq]/) {
	print STDERR "Just run ($ENV{CSCOUT_SPY_REAL_AR} @ARGV))\n" if ($debug);
	$exit = system(($ENV{CSCOUT_SPY_REAL_AR}, @ARGV)) / 256;
	print STDERR "Just run done ($exit)\n" if ($debug);
	exit $exit;
}

# Remove archive name
shift @ARGV2 if ($op =~ m/[abi]/);

# Remove count
shift @ARGV2 if ($op =~ m/N/);

$archive = shift @ARGV2;

@ofiles = @ARGV2;

if ($#afiles >= 0) {
	print RULES "BEGIN AR\n";
	print RULES "CMDLINE $origline\n";
	print RULES "OUTAR $archive\n";
	for $ofile (@ofiles) {
		print RULES "INOBJ " . abs_path($ofile) . "\n";
	}
	print RULES "END AR\n";
}

# Finally, execute the real ar
print STDERR "Finally run ($ENV{CSCOUT_SPY_REAL_AR} @ARGV))\n" if ($debug);
exit system(($ENV{CSCOUT_SPY_REAL_AR}, @ARGV)) / 256;
