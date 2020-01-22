#!/usr/bin/env perl
#
# (C) Copyright 2006-2020 Diomidis Spinellis
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
# Run make with gcc, cc, ld, ar, mv replaced with spying versions
# included in this file.
# Also, when invoked as cscc, run cc replaced with its spying version.
# Create a CScout-compatible make.cs file
#
#

$debug = 0;

use Cwd 'abs_path';
use Getopt::Std;

# Used for reading the source of the other spy programs
$script_name =$0;

# Find and set the installation directory
if (-d '.cscout') {
	$instdir = '.cscout';
} elsif (defined($ENV{CSCOUT_HOME})) {
	$instdir = $ENV{CSCOUT_HOME};
} elsif (defined($ENV{HOME}) && -d $ENV{HOME} . '/.cscout') {
	$instdir = $ENV{HOME} . '/.cscout';
} elsif (-d 'INSTALL_INCLUDE') {
	$instdir = 'INSTALL_INCLUDE';
} else {
	print STDERR "Unable to identify a CScout installation directory\n";
	print STDERR 'Create ./.cscout, or $HOME/.cscout, or set the $CSCOUT_HOME variable' . "\n";
	exit(1);
}

if (!-r ($f = "$instdir/csmake-pre-defs.h")) {
	print STDERR "Unable to read $f: $!\n";
	print STDERR "Create the file in the directory $instdir\nby copying the appropriate compiler-specific file\n";
	exit(1);
}
$instdir = abs_path($instdir);

# Copy arguments into TEMP_ARGV to use them with real make
my @TEMP_ARGV = @ARGV;
# Parse CSMAKEFLAGS
if ($ENV{'CSMAKEFLAGS'}) {
	@ARGV = split(/\s+/, $ENV{'CSMAKEFLAGS'});
}
my %options=();  # csmake options
my $csmake_opts = "hdkAs:N:T:";
getopts($csmake_opts, \%options);

my ($index) = grep { $TEMP_ARGV[$_] eq '--' } (0 .. @TEMP_ARGV-1);
my @MAKEARGS;

# Parse command line arguments and separate them into csmake arguments and make arguments.
if ($index > 0) {
    foreach my $i (0 .. $#TEMP_ARGV) {
        if ($i < $index) {
            push(@ARGV, $TEMP_ARGV[$i]);
        } elsif ($i > $index) {
            push(@MAKEARGS, $TEMP_ARGV[$i]);
        }
    }
}
getopts($csmake_opts, \%options);

if (defined $options{d}) {
	$debug = 1;
}

if (defined $options{h}) {
print <<HELP;
usage: csmake [ [-A] [-d] [-k] [-s cs_files_directory] [-T spy_directory] [-N rules_file] [-h] -- ] [make(1) options]
    -d                      Run debug mode (it also keeps spy directory in place).
    -h                      Prints help message.
    -k                      Keep temporary directory in place.
    -s cs_files_directory   Create a separate CScout .cs file for each real executable.
    -A                      Generate cs projects for static libraries.
    -N rules_file           Run on an existing rules file.
    -T spy_directory        Set spy directory.
HELP
exit();
}

if ($0 =~ m/\bcscc$/) {
	# Run as a C compiler invocation
	prepare_spy_environment($options{T});
	spy('gcc', 'spy-gcc');
	system(("gcc", @MAKEARGS));
	push(@toclean, 'rules');
	open(IN, "$ENV{CSCOUT_SPY_TMPDIR}/rules") || die "Unable to open $ENV{CSCOUT_SPY_TMPDIR}/rules for reading: $!\nMake sure you have specified appropriate compiler options.\n";
} elsif (defined $options{N}) {
	# Run on an existing rules file
	open(IN, $options{N}) || die "Unable to open $options{N} for reading: $!\n";
} else {
	prepare_spy_environment($options{T});
	spy('gcc', 'spy-gcc');
	spy('cc', 'spy-gcc');
	spy('clang', 'spy-gcc');
	spy('x86_64-linux-gnu-gcc', 'spy-gcc');
	spy($ENV{'CC'}, 'spy-gcc') if defined($ENV{'CC'});
	spy('ld', 'spy-ld');
	spy('ar', 'spy-ar');
	spy('mv', 'spy-mv');
	spy('install', 'spy-install');
	system(("make", @MAKEARGS));
	push(@toclean, 'rules');
	if (!open(IN, "$ENV{CSCOUT_SPY_TMPDIR}/rules")) {
		print STDERR "Warning: Unable to open $ENV{CSCOUT_SPY_TMPDIR}/rules for reading: $!\nMake sure a make command that creates an executable will precede or follow\nthis run.\n";
		exit 0;
	}
}

# Create a CScout .cs file
open(OUT, ">make.cs") || die "Unable to open make.cs for writing: $!\n";
# Create a Directory to save CScout .cs files for each project
my $directory = "";
if (defined $options{s}) {
	$directory = $options{s};
	if (! -e $directory) {
		unless(mkdir $directory) {
			die "Unable to create $directory\n";
		}
	}
}
# Find all install rules and save them to a hash
my %irules;
while (<IN>) {
	chop;
	if (/^INSTALL /) {
		($dummy, $excecutable, $path) = split;
		if (exists $irules{$excecutable}) {
			push @{ $irules{$excecutable} }, $path;
		} else {
			$irules{$excecutable} = [ $path ];
		}
		next;
    }
}
seek (IN, 0, 0);
# Read a spy-generated rules file
while (<IN>) {
	chop;
	if (/^RENAME /) {
		($dummy, $old, $new) = split;
		$old{$new} =$old;
		next;
	} elsif (/^BEGIN COMPILE/) {
		$state = 'COMPILE';
		undef @rules;
		undef $src;
		undef $process;
		undef $obj;
		next;
	} elsif (/^BEGIN LINK/) {
		$state = 'LINK';
		undef $exe;
		undef @obj;
		next;
	} elsif (/^BEGIN AR/) {
		$state = 'AR';
		undef $lib;
		undef @obj;
		next;
	}
	if ($state eq 'COMPILE') {
		if (/^END COMPILE/) {
			die "Missing  source in rules file" unless defined ($src);
			die "Missing object in rules file" unless defined ($obj);
			# Allow for multiple rules for the same object
			$cd = $src;
			$cd =~ s,/[^/]+$,,;
			$rules{$obj} .= qq{
#pragma echo "Processing $src\\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_include
#pragma pushd "$cd"
#include "$instdir/csmake-pre-defs.h"
} . join("\n", @rules) . qq{
#include "$instdir/csmake-post-defs.h"
$process
#pragma popd
#pragma echo "Done processing $src\\n"
#pragma block_exit
};
			undef $state;
		} elsif (/^INSRC (.*)/) {
			$src = $1;
			$process .= qq{#pragma process "$src"\n};
		} elsif (/^INMACRO (.*)/) {
			$process .= qq{#include "$1"\n};
		} elsif (/^OUTOBJ (.*)/) {
			$obj = $1;
		} elsif (/^CMDLINE/) {
			;
		} else {
			push(@rules, $_);
		}
	} elsif ($state eq 'LINK') {
		if (/^END LINK/) {
			die "Missing object in rules file" unless defined ($exe);
			if ($exe =~ m/\.[oa]$/) {
				# Output is a library or combined object file; just remember the rules
				undef $rule;
				for $o (@obj) {
					$o = ancestor($o);
					print STDERR "Warning: No compilation rule for $o\n" unless defined ($rules{$o});
					$rule .= $rules{$o};
				}
				$rules{$exe} = $rule;
			} else {
				# Output is a real executable; start a project
				create_project($exe);
			}
			undef $state;
		} elsif (/^CMDLINE/) {
			;
		} elsif (/^OUTEXE (.*)/) {
			$exe = $1;
		} elsif (/^INOBJ (.*)/ || /^INLIB (.*)/) {
			push(@obj, $1);
		}
	} elsif ($state eq 'AR') {
		if (/^END AR/) {
			die "Missing library in rules file" unless defined ($lib);
			# Output is a library; just remember the rules
			undef $rule;
			for $o (@obj) {
				$o = ancestor($o);
				print STDERR "Warning: No compilation rule for $o\n" unless defined ($rules{$o});
				$rule .= $rules{$o};
			}
			$rules{$lib} = $rule;
			if (defined $options{A}) {
				create_project($lib);
			}
			undef $state;
		} elsif (/^CMDLINE/) {
			;
		} elsif (/^OUTAR (.*)/) {
			$lib = $1;
		} elsif (/^INOBJ (.*)/) {
			push(@obj, $1);
		}
	}
}

if ($debug || defined $options{k}) {
	print "Leaving temporary files in $ENV{CSCOUT_SPY_TMPDIR}/$fname\n";
} else {
	# Clean temporary files
	for $fname (@toclean) {
		unlink("$ENV{CSCOUT_SPY_TMPDIR}/$fname");
	}
	rmdir($ENV{CSCOUT_SPY_TMPDIR});
}

exit 0;

# Print to many filehandles
sub
print_to_many
{
	my @args = @_;
	my $output = pop @args;
	foreach (@args) {
		print $_ $output;
	}
}

# Canonicalize filename
# Replace '/' or '\' with '#'
sub
can_filename
{
	my ($filename) = @_;
	$filename =~ tr|\\\/|##|;
	return ($filename);
}


# Prepare an environment for spying on command invocations
sub
prepare_spy_environment
{
    my ($option_T) = @_;
	if (defined $option_T) {
        if (! -d $option_T) {
            die "$option_T directory doesn't exist\n";
        }
		$ENV{CSCOUT_SPY_TMPDIR} = $option_T;
	} else {
		$ENV{CSCOUT_SPY_TMPDIR} = ($ENV{TMP} ? $ENV{TMP} : "/tmp") . "/spy-make.$$";
		mkdir($ENV{CSCOUT_SPY_TMPDIR}) || die "Unable to mkdir $ENV{CSCOUT_SPY_TMPDIR}: $!\n";
	}

	push(@toclean, 'empty.c');
	open(OUT, ">$ENV{CSCOUT_SPY_TMPDIR}/empty.c") || die "Unable to open $ENV{CSCOUT_SPY_TMPDIR}/empty.c for writing: $!\n";
	close(OUT);
	$ENV{PATH} = "$ENV{CSCOUT_SPY_TMPDIR}:$ENV{PATH}";
}

# Setup the environment to call spyProgName, instead of realProgName
# realProgName should be in the path
# spyProgName should be listed in this file in a BEGIN/END block
sub spy
{
	my($realProgName, $spyProgName) = @_;
	open(IN, $script_name) || die "Unable to open $script_name for reading: $!\n";
	push(@toclean, $realProgName);
	open(OUT, ">$ENV{CSCOUT_SPY_TMPDIR}/$realProgName") || die "Unable to open $ENV{CSCOUT_SPY_TMPDIR}/$realProgName for writing: $!\n";
	print OUT '#!/usr/bin/env perl
#
# Automatically-generated file
#
#

my $rulesfile = "$ENV{CSCOUT_SPY_TMPDIR}/rules";
open(RULES, ">>", $rulesfile) || die "Unable to open $rulesfile: $!\n";

# Disable buffering to avoid (for POSIX-compliant filesystems) garbled output
# from concurrent spy executions
my $ofh = select RULES;
$| = 1;
select $ofh;

# String to accumulate rules, so that they can be written with an atomic write
my $rules;

';
	print OUT "\$debug = $debug;\n";
	while (<IN>) {
		print OUT if (/^\#\@BEGIN $spyProgName/../^\#\@END/);
		print OUT if (/^\#\@BEGIN COMMON/../^\#\@END/);
	}
	close IN;
	close OUT;
	chmod(0755, "$ENV{CSCOUT_SPY_TMPDIR}/$realProgName");
}

# Return a file's original ancestor following a chain of renames
sub ancestor
{
	my($name) = @_;

	my($n) = $name;
	while (defined($old{$name})) {
		$name = $old{$name};
	}
	return ($name);
}

# Generate a CScout project.
sub
create_project
{
    my($name) = @_;
    my $install_paths = "";
    if (exists $irules{$name}) {
        @paths = @{ $irules{$name} };
        foreach (@paths) {
            $install_paths .= "\n#pragma install \"$_\"";
        }
    }
    # Create a CScout .cs file for current project
    my $filename = can_filename($name);
    if (defined $options{s}) {
        open(PROJ_OUT, ">$directory/$filename.cs") || die "Unable to open $directory/$filename.cs for writing: $!\n";
    }
    my $pragma_project_begin = qq{
#pragma echo "Processing project $name\\n"
#pragma project "$name"
#pragma block_enter$install_paths
};
    defined $options{s} ? print_to_many(OUT, PROJ_OUT, $pragma_project_begin) : print OUT $pragma_project_begin;
    for $o (@obj) {
        $o = ancestor($o);
        print STDERR "Warning: No compilation rule for $o\n" unless defined ($rules{$o});
        defined $options{s} ? print_to_many(OUT, PROJ_OUT, $rules{$o}) : print OUT $rules{$o};
    }
    my $pragma_project_end = qq{
#pragma block_exit
#pragma echo "Done processing project $name\\n\\n"
};
    defined $options{s} ? print_to_many(OUT, PROJ_OUT, $pragma_project_end) : print OUT $pragma_project_end;
    if (defined $options{s}) {
        close PROJ_OUT;
    }
}


#@END

#@BEGIN spy-ar
#
# Spy on ar invocations and construct corresponding CScout directives
#

$real = which($0);

$origline = "ar " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

@ARGV2 = @ARGV;
$op = shift @ARGV2;

if ($op !~ m/[rmq]/) {
	print STDERR "Just run ($real @ARGV))\n" if ($debug);
	$exit = system(($real, @ARGV)) / 256;
	print STDERR "Just run done ($exit)\n" if ($debug);
	exit $exit;
}

# Remove archive name
shift @ARGV2 if ($op =~ m/[abi]/);

# Remove count
shift @ARGV2 if ($op =~ m/N/);

$archive = shift @ARGV2;

@ofiles = @ARGV2;

if ($#ofiles >= 0) {
	$rules .= "BEGIN AR\n";
	$rules .= "CMDLINE $origline\n";
	$rules .= 'OUTAR ' . robust_abs_path($archive) . "\n";
	for $ofile (@ofiles) {
		$rules .= "INOBJ " . abs_path($ofile) . "\n";
	}
	$rules .= "END AR\n";
}

print RULES $rules;
close(RULES);

# Finally, execute the real ar
print STDERR "Finally run ($real @ARGV))\n" if ($debug);
exit system(($real, @ARGV)) / 256;

#@END

#@BEGIN spy-gcc
#
# Spy on gcc invocations and construct corresponding CScout directives
#
# Parsing the switches appears to be a tar pit (see incomplete version 1.1).
# Documentation is incomplete and inconsistent with the source code.
# Different gcc versions and installations add and remove options.
# Therefore it is easier to let gcc do the work
#

$real = which($0);

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
	} elsif ($arg eq '-include' || $arg eq '--include') {
		push(@incfiles, 'INSRC ' . abs_if_exists($ARGV[++$i]));
		next;
	} elsif ($arg eq '-imacros' || $arg eq '--imacros') {
		push(@incfiles, 'INMACRO ' . abs_if_exists($ARGV[++$i]));
		next;
	} elsif ($arg =~ m/\.(o|obj)$/i) {
		push(@ofiles, $arg);
		next;
	} elsif ($arg =~ m/\.(a|so)$/i) {
		push(@afiles, $arg);
		next;
	} elsif ($arg =~ m/^\-M[TF]$/) {
		# Output or target of dependency; skip it for ARGV2
		$i++;
		next;
	} else {
		push(@ARGV2, $arg) unless ($arg =~ m/^\-M/);
	}
	# Write dependencies
	# Some builds do that while compiling, so we can't just bail out
	$depwrite = 1 if (
		($arg =~ m/^-M/ || $arg =~ m/-dependencies/) &&
		$arg !~ '-MD' && $arg !~ '-MMD' &&
		$arg !~ "--write-dependencies" &&
		$arg !~ "--write-user-dependencies");
	$bailout = 1 if ($arg eq '--preprocess' || $arg eq '-E');
	$bailout = 1 if ($arg eq '--assemble' || $arg eq '-S');
	$bailout = 1 if ($arg =~ m/-print-file-name/);
	$compile = 1 if ($arg eq '--compile' || $arg eq '-c');
}

$bailout = 1 if ($depwrite && !$compile);

# We don't handle assembly files or preprocessing
if ($bailout) {
	print STDERR "Just run ($real @ARGV))\n" if ($debug);
	$exit = system(($real, @ARGV)) / 256;
	print STDERR "Just run done ($exit)\n" if ($debug);
	exit $exit;
}

if ($#cfiles >= 0) {
	push(@ARGV2, $ENV{CSCOUT_SPY_TMPDIR} . '/empty.c');
	# Escape shell metacharacters
	for ($i = 0; $i < $#ARGV2; $i++) {
		$ARGV2[$i] =~ s/([][*\\"';|()<> \t\n{}&])/\\$1/g;
	}
	$cmdline = $real . ' ' . join(' ', @ARGV2);
	print STDERR "Running $cmdline\n" if ($debug);


	# Gather include path
	open(IN, "$cmdline -v -E 2>&1|") || die "Unable to run $cmdline: $!\n";
	undef $gather;
	while (<IN>) {
		chop;
		$gather = 1 if (/\#include \"\.\.\.\" search starts here\:/);
		next if (/ search starts here\:/);
		last if (/End of search list\./);
		if ($gather) {
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

$origline = "gcc " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

# Output compilation rules
for $cfile (@cfiles) {
	$rules .= "BEGIN COMPILE\n";
	$rules .= "CMDLINE $origline\n";
	for $if (@incfiles) {
		$rules .= "$if\n";
	}
	$rules .= "INSRC " . abs_path($cfile) . "\n";
	if ($compile) {
		if ($output) {	# cc -c -o foo.o foo.c
			$rules .= "OUTOBJ " . robust_abs_path($output) . "\n";
		} else {	# cc -c foo.c
			my $coutput= $cfile;
			$coutput =~ s/\.c$/.o/i;
			$coutput =~ s,.*/,,;
			$rules .= "OUTOBJ " . robust_abs_path($coutput) . "\n";
		}
	} else {		# cc -o foo foo.c OR cc foo.c
		# Implicit output file
		# Obtain a globally unique identifier across all csmake spy
		# program runs by using the current rules file offset. This
		# will be always increasing across all programs.
		my $ofile = '/tmp/csmake-ofile-' . tell(RULES) . '.o';
		push(@implicit_ofiles, $ofile);
		$rules .= "OUTOBJ $ofile\n";
	}
	$rules .= join("\n", @incs) . "\n";
	$rules .= join("\n", @defs) . "\n";
	$rules .= "END COMPILE\n";
}

if (!$compile && !$depwrite && ($#ofiles >= 0 || $#implicit_ofiles >= 0 || $#afiles >= 0)) {

	$rules .= "BEGIN LINK\n";
	$rules .= "CMDLINE $origline\n";
	if ($output) {
		$rules .= "OUTEXE $output\n";
	} else {
		$rules .= "OUTEXE a.out\n";
	}
	for $ofile (@ofiles) {
		$rules .= "INOBJ " . abs_path($ofile) . "\n";
	}
	for $ofile (@implicit_ofiles) {
		$rules .= "INOBJ $ofile\n";
	}
	for $afile (@afiles) {
		$rules .= "INLIB " . abs_path($afile) . "\n";
	}
	for $libfile (@libs) {
		for $ldir (@ldirs) {
			if (-r ($try = "$ldir/lib$libfile.a")) {
				$rules .= "INLIB " . abs_path($try) . "\n";
				last;
			}
		}
	}
	$rules .= "END LINK\n";
}

print RULES $rules;
close(RULES);

# Finally, execute the real gcc
print STDERR "Finally run ($real @ARGV))\n" if ($debug);
exit system(($real, @ARGV)) / 256;

# Return the absolute file name of a file, if the file exists in the
# current directory
sub abs_if_exists
{
	my($fname) = @_;
	return (-r $fname) ? abs_path($fname) : $fname;
}

#@END

#@BEGIN spy-ld
#
# Spy on ld invocations and construct corresponding CScout directives
#

$real = which($0);

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
	} elsif ($arg =~ m/\.(a|so)$/i) {
		push(@afiles, $arg);
		next;
	} else {
		push(@ARGV2, $arg);
	}
}

$origline = "ld " . join(' ', @ARGV);
$origline =~ s/\n/ /g;


if ($#ofiles >= 0 || $#afiles >= 0) {
	$rules .= "BEGIN LINK\n";
	$rules .= "CMDLINE $origline\n";
	if ($output) {
		$rules .= 'OUTEXE ' . robust_abs_path($output) . "\n";
	} else {
		$rules .= 'OUTEXE ' . robust_abs_path('a.out') . "\n";
	}
	for $ofile (@ofiles) {
		$rules .= 'INOBJ ' . abs_path($ofile) . "\n";
	}
	for $afile (@afiles) {
		$rules .= 'INLIB ' . abs_path($afile) . "\n";
	}
	for $libfile (@libs) {
		for $ldir (@ldirs) {
			if (-r ($try = "$ldir/lib$libfile.a")) {
				$rules .= "INLIB " . abs_path($try) . "\n";
				last;
			}
		}
	}
	$rules .= "END LINK\n";
}

print RULES $rules;
close(RULES);

# Finally, execute the real ld
print STDERR "Finally run ($real @ARGV))\n" if ($debug);
exit system(($real, @ARGV)) / 256;

#@END

#@BEGIN spy-mv
#
# Spy on ar invocations and construct corresponding CScout directives
#

$real = which($0);

$origline = "mv " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

@ARGV2 = @ARGV;

# Remove options
while ($ARGV2[0] =~ m/^-/) {
	shift @ARGV2;
}

$rules .= "RENAMELINE $origline\n";
if ($#ARGV2 == 1) {
	$rules .= 'RENAME ' . abs_path($ARGV2[0]) . ' ' . robust_abs_path($ARGV2[1]) . "\n";
} else {
	$dir = pop(@ARGV2);
	for $f (@ARGV2) {
		$rules .= 'RENAME ' . abs_path($f) . ' ' . robust_abs_path("$dir/$f") . "\n";
	}
}

print RULES $rules;
close(RULES);

# Finally, execute the real mv
print STDERR "Finally run ($real @ARGV))\n" if ($debug);
exit system(($real, @ARGV)) / 256;
#@END

#@BEGIN spy-install
#
# Spy on install invocations to detect where each file will be installed,
# and construct corresponding CScout directives
#

$real = which($0);

$origline = "install " . join(' ', @ARGV);
$origline =~ s/\n/ /g;

@ARGV2 = @ARGV;

my @excecutables = ();
my $dest;
my $length = @ARGV2;
my $counter = 0;

while (my $i = shift @ARGV2) {
	$counter++;
	# If -d option is used skip this command
	if ($i eq "-d") {
		last;
	}
	# If -t save next argument to directory
	if ($i eq "-t") {
		$dest = shift @ARGV2;
		$counter++;
	}
	if ($counter == $length) {
		if (length $dest) {
			if (-x $i && ! -d $i) {
				push(@excecutables, $i);
			}
		} else {
			$dest = $i;
		}
		last;
	}
	# If file is a real executable add it to array
	if (-x $i && ! -d $i) {
		push(@excecutables, $i);
	}
}

if (@excecutables) {
	foreach (@excecutables) {
		$rules .= "INSTALL $_ $dest\n";
	}
}

print RULES $rules;
close(RULES);

# Finally, execute the real install
print STDERR "Finally run ($real @ARGV))\n" if ($debug);
exit system(($real, @ARGV)) / 256;
#@END

#@BEGIN COMMON
#
# Code common to all spy programs
#

use Cwd 'abs_path';

# Return the absolute path of a file, even if does not exist
# Under Cygwin Perl, abs_path on missing files aborts the program
# with an error
sub
robust_abs_path
{
	my ($file) = @_;
	if (-r $file) {
		return abs_path($file);
	} else {
		my ($ret);
		open my $out, '>', $file or die "Unable to create $file: $!\n";
		close $out;
		$ret = abs_path($file);
		unlink($file);
		return $ret;
	}
}

# Return the absolute path for prog, excluding our own path
sub which
{
	my ($prog) = @_;
	$prog =~ s,^.*/,,;
	my @dirs = split(/:/, $ENV{PATH});
	for my $d (@dirs) {
		next if ($d eq $ENV{CSCOUT_SPY_TMPDIR} || $d =~ m/ccache/);
		return "$d/$prog" if (-x "$d/$prog");
	}
	die "Unable to locate $prog in PATH $ENV{PATH}\n";
}

#@END
