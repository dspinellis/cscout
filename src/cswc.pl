#!/usr/bin/env perl
#
# (C) Copyright 2003-2019 Diomidis Spinellis
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
# Compile a project description into a C-file compilation script
#
#

# Syntax:
#
## Comment
#workspace name {
#	cd "dirname"
#	define foo bar
#	ro_prefix "path1"
#	ro_prefix "path2"
#	ipath "blah"
#	ipath "foobar"
#	readonly
#	readonly "fname"
#	# Linkage unit
#	project name {
#		cd ...
#		define ...
#		ipath ...
#		readonly
#		directory name {
#			readonly
#			file "fname.c" {
#				cd ...
#				define ...
#				ipath ...
#				readonly
#			}
#		}
#		# Or:
#		file fname2.c ...
#	}
#}
#

use Getopt::Std;
if (!getopts('d:gv')) {
	print STDERR "usage: $0 [-gv] [-d directory] [file]\n";
	exit(1);
}

if ($opt_v) {
	print STDERR "cswc - CScout workspace compiler\n\n" .
	# 80 column terminal width----------------------------------------------------
	"(C) Copyright 2003-2019 Diomidis Spinelllis.\n\n" .
	"CScout is distributed as open source software under the GNU General Public\n" .
	"License, available in the CScout documentation and online at\n" .
	"http://www.gnu.org/licenses/.\n" .
	"Other licensing options and professional support are available on request.\n";
	exit(0);
}

# Installation directory:
# Search order
# $opt_d, .cscout, $CSCOUT_HOME, $HOME/.cscout, and
# INSTALL_INCLUDE
use Cwd 'abs_path';

if (defined($opt_d) && -d $opt_d) {
	$instdir = $opt_d;
} elsif (-d '.cscout') {
	$instdir = '.cscout';
} elsif (defined($ENV{CSCOUT_HOME})) {
	$instdir = $ENV{CSCOUT_HOME};
} elsif (defined($ENV{HOME}) && -d $ENV{HOME} . '/.cscout') {
	$instdir = $ENV{HOME} . '/.cscout';
} elsif (-d 'INSTALL_INCLUDE') {
	$instdir = 'INSTALL_INCLUDE';
} else {
	print STDERR "Unable to identify a CScout installation directory\n";
	print STDERR 'Create ./.cscout, or $HOME/.cscout, use -d, or set the $CSCOUT_HOME variable' . "\n";
	exit(1);
}
$instdir = abs_path($instdir);

my ($incs, $defs);

if (defined($opt_g)) {
	$incs = 'stdc-incs.h';
	$defs = 'stdc-defs.h';
} else {
	$incs = 'host-incs.h';
	$defs = 'host-defs.h';
}

if (!-r ($f = "$instdir/$incs")) {
	print STDERR "Unable to read $f: $!\n";
	print STDERR "Create the file in the directory $instdir\nby copying the appropriate compiler-specific file\n";
	exit(1);
}

if (!-r ($f = "$instdir/$defs")) {
	print STDERR "Unable to read $f: $!\n";
	print STDERR "Create the file in the directory $instdir\nby copying the appropriate compiler-specific file\n";
	exit(1);
}

if ($#ARGV == -1) {
	print STDERR "Reading workspace decription from the standard input\n";
}

while (<>) {
	chop;
	s/#.*//;
	s/^\s+//;
	s/\s+$//;
	next if (/^$/);
	if (/(^workspace|project|file|directory)\s+([^ ]+)\s*\{$/) {
		beginunit($1, $2);
	} elsif (/^file\s+(.*[^{])$/) {
		@files = split(/\s+/, $1);
		for $f (@files) {
			beginunit('file', $f);
			endunit();
		}
	} elsif (/^ro_prefix\s+(\".*\")$/) {
		print "#pragma ro_prefix $1\n";
	} elsif (/^cd\s+\"(.*)\"$/) {
		directory($1);
	} elsif (/^define\s+(\w+)$/) {
		$defines{$unit . $name} .= "#define $1\n";
		# print "DEBUG define $1 ($unit $name)\n";
	} elsif (/^define\s+(\w+)\s*(.*)$/) {
		$defines{$unit . $name} .= "#define $1 $2\n";
		# print "DEBUG define $1 $2 ($unit $name)\n";
	} elsif (/^ipath\s+(\".*\")$/) {
		$ipaths{$unit . $name} .= "#pragma includepath $1\n";
	} elsif (/^readonly\s+(\".*\")$/) {
		print "#pragma readonly $1\n";
	} elsif (/^readonly$/) {
		$readonly{$unit . $name} = 1;
	} elsif (/^\}$/) {
		endunit();
	} else {
		print STDERR "Syntax error: $_\n";
	}
}

sub endunit
{
	if ($unit eq 'file') {
		my $i;
		# Include current ones in the scope
		push(@units, $unit);
		push(@names, $name);
		for ($i = 0; $i <= $#units; $i++) {
			my $u = $units[$i];
			my $n = $names[$i];
			print $ipaths{$u . $n};
			print $defines{$u . $n};
			# print "DEBUG looking defines for $u $n\n";
			print "#pragma readonly \"$name\" // $u\n" if ($readonly{$u . $n});
		}
		# Current ones no longer needed
		pop(@units);
		pop(@names);
		print "#include \"$instdir/$incs\"\n";
		print "#pragma process \"$name\"\n\n";
	}
	if (defined($dir{$unit})) {
		print "#pragma echo \"Exiting directory $dir{$unit}\\n\"\n";
		print "#pragma popd\n";
	}
	print "#pragma block_exit\n" unless($unit eq 'workspace' || $unit eq 'directory');
	print "#pragma echo \"Done processing $unit $name\\n\"\n";
	$unit = pop(@units);
	$name = pop(@names);
	$readonly{$unit . $name} = pop(@readonlys);
}


sub beginunit
{
	push(@units, $unit);
	push(@names, $name);
	push(@readonlys, $readonly{$unit . $name});
	($unit, $name) = @_;
	# print "DEBUG beginunit $unit $name\n";
	undef $dir{$unit};
	undef $defines{$unit . $name};
	undef $ipaths{$unit . $name};
	undef $readonly{$unit . $name};
	print "// $unit $name\n";
	print "#pragma echo \"Processing $unit $name\\n\"\n";
	if ($unit eq 'project') {
		print "#pragma project \"$name\"\n";
		print "#pragma block_enter\n";
	} elsif ($unit eq 'directory') {
		directory($name);
	} elsif ($unit eq 'file') {
		print "#pragma block_enter\n";
		print "#pragma clear_defines\n";
		print "#pragma clear_include\n";
		print "#pragma process \"$instdir/$defs\"\n";
	}
}

sub directory
{
	my($dir) = @_;
	$dir{$unit} = $dir;
	print "#pragma echo \"Entering directory $dir\\n\"\n";
	print "#pragma pushd \"$dir\"\n";
}
