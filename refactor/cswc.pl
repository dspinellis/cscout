#!/usr/bin/perl
#
# Compile a project description into a C-file compilation script
#
# $Id: cswc.pl,v 1.11 2003/06/22 23:33:34 dds Exp $
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

if ($ARGV[0] eq '-E') {
	$cpp = 1;
	shift @ARGV;
}

# Installation directory:
# Search order
# .cscout, $CSCOUT_HOME, and $HOME/.cscout
if (-d '.cscout') {
	$instdir = '.cscout';
} elsif (defined($ENV{CSCOUT_HOME})) {
	$instdir = $ENV{CSCOUT_HOME};
} elsif (defined($ENV{HOME}) && -d $ENV{HOME} . '/.cscout') {
	$instdir = $ENV{HOME} . '/.cscout';
} else {
	print STDERR "Unable to identify a cscout installation directory\n";
	print STDERR 'Create ./.cscout, or $HOME/.cscout or set the $CSCOUT_HOME variable\n';
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
		if ($cpp) {
			print "#include \"$name\"\n\n";
		} else {
			print "#pragma process \"$name\"\n\n";
		}
	}
	if (defined($dir{$unit})) {
		print "#pragma echo \"Exiting directory $dir{$unit}\\n\"\n" unless ($cpp);
		print "#pragma popd\n";
	}
	print "#pragma block_exit\n" unless($unit eq 'workspace' || $unit eq 'directory');
	print "#pragma echo \"Done processing $unit $name\\n\"\n" unless ($cpp);
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
	print "#pragma echo \"Processing $unit $name\\n\"\n" unless ($cpp);
	if ($unit eq 'project') {
		print "#pragma project \"$name\"\n";
		print "#pragma block_enter\n";
	} elsif ($unit eq 'directory') {
		directory($name);
	} elsif ($unit eq 'file') {
		print "#pragma block_enter\n";
		print "#pragma clear_defines\n";
		print "#pragma clear_include\n";
		print "#include \"$instdir/cscout_defs.h\"\n";
	}
}

sub directory
{
	my($dir) = @_;
	$dir{$unit} = $dir;
	print "#pragma echo \"Entering directory $dir\n\"\n" unless ($cpp);
	print "#pragma pushd \"$dir\"\n";
}
