#!/usr/bin/perl
#
# (C) Copyright 2003-2006 Diomidis Spinellis
#
# This software is distributed under the terms of the CScout Public License
# The license is available as part of the software documentation, both
# in the distribution package and online http://www.spinellis.gr/cscout
#
# THE SOFTWARE AND ITS DOCUMENTATION ARE PROVIDED ``AS IS'' AND WITHOUT
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION,
# THE IMPLIED WARRANTIES OF DESIGN, MERCHANTIBILITY, OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Compile a project description into a C-file compilation script
#
# $Id: cswc.pl,v 1.14 2006/09/30 20:03:22 dds Exp $
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
#	readonly "fname"
#	# Linkage unit
#	project name {
#		cd ...
#		define ...
#		ipath ...
#		directory name {
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
if (!getopts('vEd:')) {
	print STDERR "usage: $0 [-vE] [-d directory] [file]\n";
	exit(1);
}

if ($opt_v) {
	my $rel = '$Revision: 1.14 $';
	$rel =~ s/\//;
	$rel =~ s/\$//;
	print STDERR "cswc - CScout workspace compiler - version $rel\n\n" .
	# 80 column terminal width----------------------------------------------------
	"(C) Copyright 2003-2006 Diomidis Spinelllis.\n\n" .
	"Unsupported version.  Can be used and distributed under the terms of the\n" .
	"CScout Public License available in the CScout documentation and online at\n" .
	"http://www.spinellis.gr/cscout/doc/license.html\n";
	exit(0);
}

# Installation directory:
# Search order
# $opt_d, .cscout, $CSCOUT_HOME, and $HOME/.cscout
if (defined($opt_d) && -d $opt_d) {
	$instdir = $opt_d;
} elsif (-d '.cscout') {
	$instdir = '.cscout';
} elsif (defined($ENV{CSCOUT_HOME})) {
	$instdir = $ENV{CSCOUT_HOME};
} elsif (defined($ENV{HOME}) && -d $ENV{HOME} . '/.cscout') {
	$instdir = $ENV{HOME} . '/.cscout';
} else {
	print STDERR "Unable to identify a cscout installation directory\n";
	print STDERR 'Create ./.cscout, or $HOME/.cscout, use -d, or set the $CSCOUT_HOME variable' . "\n";
	exit(1);
}

if (!-r ($f = "$instdir/cscout_incs.h")) {
	print STDERR "Unable to read $f: $!\n";
	print STDERR "Create the file in the directory $instdir\nby copying the appropriate compiler-specific file\n";
	exit(1);
}

if (!-r ($f = "$instdir/cscout_defs.h")) {
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
	} elsif (/^define\s+(.*)$/) {
		$defines{$unit} .= "#define $1\n";
	} elsif (/^ipath\s+(\".*\")$/) {
		$ipaths{$unit} .= "#pragma includepath $1\n";
	} elsif (/^readonly\s+(\".*\")$/) {
		print "#pragma readonly $1\n";
	} elsif (/^readonly$/ && $unit eq 'file') {
		print "#pragma readonly \"$name\"\n";
	} elsif (/^\}$/) {
		endunit();
	} else {
		print STDERR "Syntax error: $_\n";
	}
}

sub endunit
{
	if ($unit eq 'file') {
		print $ipaths{'workspace'};
		print $defines{'workspace'};
		print $ipaths{'project'};
		print $defines{'project'};
		print $ipaths{'directory'};
		print $defines{'directory'};
		print $ipaths{'file'};
		print $defines{'file'};
		print "#include \"$instdir/cscout_incs.h\"\n";
		if ($opt_E) {
			print "#include \"$name\"\n\n";
		} else {
			print "#pragma process \"$name\"\n\n";
		}
	}
	if (defined($dir{$unit})) {
		print "#pragma echo \"Exiting directory $dir{$unit}\\n\"\n" unless ($opt_E);
		print "#pragma popd\n";
	}
	print "#pragma block_exit\n" unless($unit eq 'workspace' || $unit eq 'directory');
	print "#pragma echo \"Done processing $unit $name\\n\"\n" unless ($opt_E);
	$unit = pop(@units);
	$name = pop(@names);
}


sub beginunit
{
	push(@units, $unit);
	push(@names, $name);
	($unit, $name) = @_;
	undef $dir{$unit};
	undef $defines{$unit};
	undef $ipaths{$unit};
	print "// $unit $name\n";
	print "#pragma echo \"Processing $unit $name\\n\"\n" unless ($opt_E);
	if ($unit eq 'project') {
		print "#pragma project \"$name\"\n";
		print "#pragma block_enter\n";
	} elsif ($unit eq 'directory') {
		directory($name);
	} elsif ($unit eq 'file') {
		print "#pragma block_enter\n";
		print "#pragma clear_defines\n";
		print "#pragma clear_include\n";
		if ($opt_E) {
			print "#include \"$instdir/cscout_defs.h\"\n";
		} else {
			print "#pragma process \"$instdir/cscout_defs.h\"\n";
		}
	}
}

sub directory
{
	my($dir) = @_;
	$dir{$unit} = $dir;
	print "#pragma echo \"Entering directory $dir\n\"\n" unless ($opt_E);
	print "#pragma pushd \"$dir\"\n";
}
