#!/usr/bin/perl
#
# Run make with gcc, cc, ld, ar replaced with spying versions
# Create a CScout-compatible make.cs file
#
# $Id: csmake.pl,v 1.6 2006/06/18 19:34:23 dds Exp $
#

$ENV{CSCOUT_SPY_TMPDIR} = ($ENV{TMP} ? $ENV{TMP} : "/tmp") . "/gccspy.$$";
mkdir($ENV{CSCOUT_SPY_TMPDIR}) || die "Unable to mkdir $ENV{CSCOUT_SPY_TMPDIR}: $!\n";

open(OUT, ">$ENV{CSCOUT_SPY_TMPDIR}/empty.c") || die;
close(OUT);
$ENV{PATH} = "$ENV{CSCOUT_SPY_TMPDIR}:$ENV{PATH}";

spy('gcc', 'spy-gcc');
spy('cc', 'spy-gcc');
spy('ld', 'spy-ld');
spy('ar', 'spy-ar');

if ($ARGV[0] eq '-n') {
	# Run on an existing rules file
	open(IN, $ARGV[1]) || die;
} else {
	system(("make", @ARGV));
	open(IN, "$ENV{CSCOUT_SPY_TMPDIR}/rules") || die;
}

# Read a spy-generated rules file
# Create a CScout .cs file
open(OUT, ">make.cs") || die;
while (<IN>) {
	chop;
	if (/^BEGIN COMPILE/) {
		$state = 'COMPILE';
		undef @rules;
		undef $src;
		undef $obj;
		next;
	} elsif (/^BEGIN LINK/) {
		$state = 'LINK';
		undef $exe;
		undef @obj;
		next;
	}
	if ($state eq 'COMPILE') {
		if (/^END COMPILE/) {
			die "No source" unless defined ($src);
			die "No object" unless defined ($obj);
			# Allow for multiple rules for the same object
			$rules{$obj} .= qq{
#pragma echo "Processing $src\\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_include
#include "cscout_defs.h"
} . join("\n", @rules) . qq{
#pragma process "$src"
#pragma echo "Done processing $src\\n"
#pragma block_exit
};
			undef $state;
		} elsif (/^INSRC (.*)/) {
			$src = $1;
		} elsif (/^OUTOBJ (.*)/) {
			$obj = $1;
		} elsif (/^CMDLINE/) {
			;
		} else {
			push(@rules, $_);
		}
	} elsif ($state eq 'LINK') {
		if (/^END LINK/) {
			die "No object" unless defined ($exe);
			if ($exe =~ m/\.o$/) {
				undef $rule;
				for $o (@obj) {
					print STDERR "Warning: No compilation rule for $o\n" unless defined ($rules{$o});
					print $rule .= $rules{$o};
				}
				$rules{$exe} = $rule;
			} else {
				print OUT qq{
#pragma echo "Processing project $exe\\n"
#pragma project "$exe"
#pragma block_enter
};
				for $o (@obj) {
					print STDERR "Warning: No compilation rule for $o\n" unless defined ($rules{$o});
					print OUT $rules{$o};
				}
				print OUT qq{
#pragma block_exit
#pragma echo "Done processing project $exe\\n\\n"
};
			}
			undef $state;
		} elsif (/^CMDLINE/) {
			;
		} elsif (/OUTEXE (.*)/) {
			$exe = $1;
		} elsif (/INOBJ (.*)/) {
			push(@obj, $1);
		}
	}
}

# Setup the environment to call spyProgName, instead of realProgName
# realProgName and spyProgName should both be in the path
sub spy
{
	my($realProgName, $spyProgName) = @_;
	$realProgPath = `which $realProgName`;
	chop $realProgPath;
	$spyProgPath = `which $spyProgName`;
	chop $spyProgPath;
	$ENV{'CSCOUT_SPY_REAL_' . uc($realProgName)} = $realProgPath;
	system("cp $spyProgPath $ENV{CSCOUT_SPY_TMPDIR}/$realProgName");
	chmod(0755, "$ENV{CSCOUT_SPY_TMPDIR}/$realProgName");
}
