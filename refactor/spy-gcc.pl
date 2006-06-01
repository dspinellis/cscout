#!/usr/bin/perl
#
# Spy on gcc invocations and construct corresponding CScout directives
#
# $Id: spy-gcc.pl,v 1.2 2006/06/01 08:06:30 dds Exp $
#
# (C) Copyright 2005 Diomidis Spinellis.  All rights reserved.
#
# Parsing the switches appears to be a tar pit (see incomplete version 1.1).
# Documentation is incomplete and inconsistent with the source code.
# Different gcc versions and installations add and remove options.
# Therefore it is easier to let gcc do the work
#

use Cwd 'abs_path';

$debug = 0;

# Gather input / output files and remove them from the command line
for ($i = 0; $i <= $#ARGV; $i++) {
	$arg = $ARGV[$i];
	if ($arg =~ m/\.c$/i) {
		push(@cfiles, $arg);
	} elsif ($arg =~ m/^-o(.+)$/ || $arg =~ m/^--output=(.*)/) {
		$output = $1;
		next;
	} elsif ($arg eq '-o' || $arg eq '--output') {
		$output = $ARGV[++$i];
		next;
	} elsif ($arg =~ m/^-L(.+)$/ || $arg =~ m/^--library-directory=(.*)/) {
		push(@ldirs, $1);
		next;
	} elsif ($arg eq '-L' || $arg eq '--library-directory') {
		push(@ldirs, $ARGV[++$i]);
		next;
	} elsif ($arg =~ m/^-l(.+)$/ || $arg =~ m/^--library=(.*)/) {
		push(@libs, $1);
		next;
	} elsif ($arg eq '-l' || $arg eq '--library') {
		push(@libs, $ARGV[++$i]);
		next;
	} elsif ($arg =~ m/\.(o|obj)$/i) {
		push(@ofiles, $arg);
		next;
	} elsif ($arg =~ m/\.a$/i) {
		push(@afiles, $arg);
		next;
	} else {
		push(@ARGV2, $arg);
	}
	$bailout = 1 if (
		($arg =~ m/^-M/ || $arg =~ m/-dependencies/) &&
		$arg !~ '-MD' && $arg !~ '-MMD' &&
		$arg !~ "--write-dependencies" &&
		$arg !~ "--write-user-dependencies");
	$bailout = 1 if ($arg eq '--preprocess' || $arg eq '-E');
	$bailout = 1 if ($arg eq '--assemble' || $arg eq '-S');
	$bailout = 1 if ($arg =~ m/-print-file-name/);
	$compile = 1 if ($arg eq '--compile' || $arg eq '-c');
}

# We don't handle assembly files or preprocessing
if ($bailout) {
	print STDERR "Just run ($ENV{CSCOUT_SPY_REAL_GCC} @ARGV))\n" if ($debug);
	$exit = system(($ENV{CSCOUT_SPY_REAL_GCC}, @ARGV)) / 256;
	print STDERR "Just run done ($exit)\n" if ($debug);
	exit $exit;
}

if ($#cfiles >= 0) {
	push(@ARGV2, $ENV{CSCOUT_SPY_TMPDIR} . '/empty.c');
	$cmdline = $ENV{CSCOUT_SPY_REAL_GCC} . ' ' . join(' ', @ARGV2);
	print STDERR "Running $cmdline\n" if ($debug);

	# Gather include path
	open(IN, "$cmdline -v -c 2>&1|") || die "Unable to run $cmdline: $!\n";
	while (<IN>) {
		chop;
		if (/\#include \"\.\.\.\" search starts here\:/ .. /End of search list\./) {
			next if (/\#include [\<\"]\.\.\.[\>\"] search starts here\:/);
			next if (/End of search list\./);
			s/^\s*//;
			push(@incs, '#pragma includepath "' . abs_path($_) . '"');
			print STDERR "path=[$_]\n" if ($debug > 2);
		}
	}

	# Gather macro definitions
	open(IN, "$cmdline -dD -E|") || die "Unable to run $cmdline: $!\n";
	while (<IN>) {
		chop;
		next if (/\s*\#\s*\d/);
		push(@defs, $_);
	}
}

open(RULES, $rulesfile = ">>$ENV{CSCOUT_SPY_TMPDIR}/rules") || die "Unable to open $rulesfile: $!\n";

$origline = "gcc " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

# Output compilation rules
for $cfile (@cfiles) {
	print RULES "BEGIN COMPILE\n";
	print RULES "CMDLINE $origline\n";
	print RULES "INSRC " . abs_path($cfile) . "\n";
	if ($compile && $output) {
		$coutput = $output;
	} else {
		$coutput= $cfile;
		$coutput =~ s/\.c$/.o/i;
		$coutput =~ s,.*/,,;
	}
	print RULES "OUTOBJ " . abs_path($coutput) . "\n";
	print RULES join("\n", @incs), "\n";
	print RULES join("\n", @defs), "\n";
	print RULES "END COMPILE\n";
}

if (!$compile && $#cfiles >= 0 || $#ofiles >= 0) {
	print RULES "BEGIN LINK\n";
	print RULES "CMDLINE $origline\n";
	if ($output) {
		print RULES "OUTEXE $output\n";
	} else {
		print RULES "OUTEXE a.out\n";
	}
	for $cfile (@cfiles) {
		$output= $cfile;
		$output =~ s/\.c$/.o/i;
		print RULES "INOBJ " . abs_path($output) . "\n";
	}
	for $ofile (@ofiles) {
		print RULES "INOBJ " . abs_path($ofile) . "\n";
	}
	for $afile (@afiles) {
		print RULES "INLIB " . abs_path($afile) . "\n";
	}
	for $libfile (@libfiles) {
		for $ldir (@ldirs) {
			if (-r ($try = "$ldir/lib$libfile.a")) {
				print RULES "INLIB " . abs_path($try) . "\n";
				last;
			}
		}
	}
	print RULES "END LINK\n";
}

# Finally, execute the real gcc
print STDERR "Finally run ($ENV{CSCOUT_SPY_REAL_GCC} @ARGV))\n" if ($debug);
exit system(($ENV{CSCOUT_SPY_REAL_GCC}, @ARGV)) / 256;
