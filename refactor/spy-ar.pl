#!/usr/bin/perl
#
# Spy on ar invocations and construct corresponding CScout directives
#
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
