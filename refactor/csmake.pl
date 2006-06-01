#!/usr/bin/perl
#
# Run make with gcc, cc, ld, ar replaced with spying versions
#
# $Id: csmake.pl,v 1.3 2006/06/01 16:22:00 dds Exp $
#

$ENV{CSCOUT_SPY_TMPDIR} = ($ENV{TMP} ? $ENV{TMP} : "/tmp") . "/gccspy.$$";
mkdir($ENV{CSCOUT_SPY_TMPDIR}) || die "Unable to mkdir $ENV{CSCOUT_SPY_TMPDIR}: $!\n";

open(OUT, ">$ENV{CSCOUT_SPY_TMPDIR}/empty.c");
close(OUT);
$ENV{PATH} = "$ENV{CSCOUT_SPY_TMPDIR}:$ENV{PATH}";

spy('gcc');
spy('cc');
spy('ld');
spy('ar');

system(("make", @ARGV));

sub spy
{
	my($prog) = @_;
	$path = `which $prog`;
	chop $path;
	$ENV{'CSCOUT_SPY_REAL_' . uc($prog)} = $path;
	system("cp spy-$prog.pl $ENV{CSCOUT_SPY_TMPDIR}/$prog");
	chmod(0755, "$ENV{CSCOUT_SPY_TMPDIR}/$prog");
}
