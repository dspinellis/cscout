#/usr/bin/perl
#
# Spy on gcc invocations and construct corresponding CScout directives
#
#
# (C) Copyright 2005 Diomidis Spinellis.  All rights reserved.
#
# Parsing the switches appears to be a tar pit (see incomplete version 1.1).
# Documentation is incomplete and inconsistent with the source code.
# Different gcc versions and installations add and remove options.
# Therefore it is easier to let gcc do the work
#

use Cwd 'abs_path';

$cmdline = join(' ', @ARGV);

$assemble = ($cmdline =~ m/--asset gcc do the work
#

use Cwd 'abs_path';

$cmdline = join(' ', @ARGV);

$assemble = ($cmdline =~ m/--assemble\b/ || $cmdline =~ m/-S\b/);

$compile = ($cmdline =~ m/--compile\b/ || $cmdline =~ m/-c\b/);

# Gather input / output files and remove them from the command line
for $i = 0; $i <= $#ARGV; $i++) {
	$arg = $ARGV[$i];
	if ($arg =~ m/\.c$/i) {
		push(@cfiles, $arg);
	} elsif ($arg =~ m/^-o(.+)$/ || $arg =~ m/^--output=(.*)/) {
		$output = $1;
	} elsif ($arg eq '-o' || $arg eq '--output') {
		$output = $ARGV[++$i];
	} elsif ($arg =~ m/^-L(.+)$/ || $arg =~ m/^--library-directory=(.*)/) {
		push(@ldirs, $1);
	} elsif ($arg eq '-L' || $arg eq '--library-directory') {
		push(@ldirs, $ARGV[++$i]);
	} elsif ($arg =~ m/^-l(.+)$/ || $arg =~ m/^--library=(.*)/) {
		push(@libs, $1);
	} elsif ($arg eq '-l' || $arg eq '--library') {
		push(@libs, $ARGV[++$i]);
	} elsif ($arg =~ m/\.(o|obj)$/i) {
		push(@ofiles, $arg);
	} elsif ($arg =~ m/\.a$/i) {
		push(@afiles, $arg);
	} else {
		push(@ARGV2, $arg);
	}
	$preprocess = 1 if ($arg eq '--preprocess' || $arg eq '-E');
	$assemble = 1 if ($arg eq '--assemble' || $arg eq '-S');
	$compile = 1 if ($arg eq '--compile' || $arg eq '-c');
}

# We don't handle assembly files or preprocessing
exit(0) if ($assemble || $preprocess);

if ($#cfiles >= 0) {
	push(@ARGV2, $ENV{GCCSPY_TMPDIR} . '/empty.c');
	$cmdline = $ENV{GCCSPY_REAL_GCC} . ' ' . join(' ', @ARGV2);

	# Gather include path
	open(IN, "$cmdline -v -c 2>&1|") || die "Unable to run $cmdline: $!\n";
	while (<IN>) {
		chop;
		if (/\#include \"\.\.\.\" search starts here\:/ .. /End of search list\./) {
			next if (/\#include \<\.\.\.\> search starts here\:/);
			push(@incs, qq{#pragma includepath "$_"});
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
}

open(RULES, $rulesfile = ">>$ENV{GCCSPY_TMPDIR}/rules") || die "Unable to open $rulesfile: $!\n";

# Output compilation rules
for $cfile (@cfiles) {
	print RULES "BEGIN COMPILE\n";
	$cfile = abs_path($cfile);
	print RULES "INC " . $abs_file($cfile) . "\n";
	if (!($compile && $output)) {
		$output= $cfile;
		$output =~ s/\.c$/.o/i;
	}
	print RULES "OUTOBJ " . $abs_file($output) . "\n";
	print RULES join("\n", @incs);
	print RULES join("\n", @defs);
	print RULES "END COMPILE\n";
}

if (!$compile) {
	print RULES "BEGIN LINK\n";
	if ($output) {
		print RULES "OUTEXE $output\n";
	} else {
		print RULES "OUTEXE a.out\n";
	}
	for $cfile (@cfiles) {
		$output= $cfile;
		$output =~ s/\.c$/.o/i;
		print RULES "INOBJ " . $abs_file($output) . "\n";
	}
	for $ofile (@ofiles) {
		print RULES "INOBJ " . $abs_file($ofile) . "\n";
	}
	for $afile (@afiles) {
		print RULES "INLIB " . $abs_file($afile) . "\n";
	}
	for $libfile (@libfiles) {
		for $ldir (@ldirs) {
			if (-r ($try = "$ldir/lib$libfile.a"))