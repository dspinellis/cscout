#!/bin/perl
#
# Compile a project description into a C-file compilation script
#
# $Id: cswc.pl,v 1.2 2002/09/05 10:38:49 dds Exp $
#

# Syntax:
#
## Comment
#workspace name {
#	cd "dirname"
#	define foo bar
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
		print "#pragma process \"$name\"\n\n";
	}
	if (defined($dir{$unit})) {
		print "#pragma echo \"Exiting directory $dir{$unit}\\n\"\n";
		print "#pragma popd\n";
	}
	print "#pragma block_exit\n" unless($unit eq 'workspace');
	print "#pragma echo \"Done processing $unit $name\\n\"\n";
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
	print "#pragma echo \"Processing $unit $name\\n\"\n";
	if ($unit eq 'project') {
		print "#pragma block_enter\n";
	} elsif ($unit eq 'directory') {
		directory($name);
	} elsif ($unit eq 'file') {
		print "#pragma block_enter\n";
		print "#pragma clear_defines\n";
		print "#pragma clear_include\n";
		print "#include \"/home/dds/src/refactor/defs.h\"\n";
	}
}

sub directory
{
	my($dir) = @_;
	$dir{$unit} = $dir;
	print "#pragma echo \"Entering directory $dir\n\"\n";
	print "#pragma pushd \"$dir\"\n";
}
