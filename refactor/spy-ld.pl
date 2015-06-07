#!/usr/bin/perl
#
# Spy on ld invocations and construct corresponding CScout directives
#
# (C) Copyright 2005 Diomidis Spinellis
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

use Cwd 'abs_path';

# Gather input / output files and remove them from the command line
for ($i = 0; $i <= $#ARGV; $i++) {
	$arg = $ARGV[$i];
	if ($arg =~ m/\.o$/i) {
		push(@ofiles, $arg);
	} elsif ($arg =~ m/^-o(.+)$/ || $arg =~ m/^--output=(.*)/) {
		$output = $1;
		next;
	} elsif ($arg eq '-o' || $arg eq '--output') {
		$output = $ARGV[++$i];
		next;
	} elsif ($arg =~ m/^-L(.+)$/ || $arg =~ m/^--library-path=(.*)/) {
		push(@ldirs, $1);
		next;
	} elsif ($arg eq '-L' || $arg eq '--library-path') {
		push(@ldirs, $ARGV[++$i]);
		next;
	} elsif ($arg =~ m/^-l(.+)$/ || $arg =~ m/^--library=(.*)/) {
		push(@libs, $1);
		next;
	} elsif ($arg eq '-l' || $arg eq '--library') {
		push(@libs, $ARGV[++$i]);
		next;
	} elsif ($arg =~ m/\.a$/i) {
		push(@afiles, $arg);
		next;
	} else {
		push(@ARGV2, $arg);
	}
}

open(RULES, $rulesfile = ">>$ENV{CSCOUT_SPY_TMPDIR}/rules") || die "Unable to open $rulesfile: $!\n";

$origline = "ld " . join(' ', @ARGV);
$origline =~ s/\n/ /g;


if ($#ofiles >= 0 || $#afiles >= 0) {
	print RULES "BEGIN LINK\n";
	print RULES "CMDLINE $origline\n";
	if ($output) {
		print RULES "OUTEXE $output\n";
	} else {
		print RULES "OUTEXE a.out\n";
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

# Finally, execute the real ld
print STDERR "Finally run ($ENV{CSCOUT_SPY_REAL_LD} @ARGV))\n" if ($debug);
exit system(($ENV{CSCOUT_SPY_REAL_LD}, @ARGV)) / 256;
