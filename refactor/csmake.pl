#!/usr/bin/perl
#
# Run make with gcc, cc, ld, ar replaced with spying versions
# Create a CScout-compatible make.cs file
#
# $Id: csmake.pl,v 1.4 2006/06/01 22:04:46 dds Exp $
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

system(("make", @ARGV));


# Read a spy-generated rules file
# Create a CScout .cs file

open(IN, "$ENV{CSCOUT_SPY_TMPDIR}/rules") || die;
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
			die "Multiple rules for $obj" if defined($rules{$obj});
			$rules{$obj} = qq{
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

sub spy
{
	my($prog, $spyprog) = @_;
	$path = `which $prog`;
	chop $path;
	$ENV{'CSCOUT_SPY_REAL_' . uc($prog)} = $path;
	system("cp $spyprog.pl $ENV{CSCOUT_SPY_TMPDIR}/$prog");
	chmod(0755, "$ENV{CSCOUT_SPY_TMPDIR}/$prog");
}
