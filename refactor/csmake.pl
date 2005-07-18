$ENV{CSCOUT_SPY_TMPDIR} = ($ENV{TMP} ? $ENV{TMP} : "/tmp") . "/gccspy.$$";
mkdir($ENV{CSCOUT_SPY_TMPDIR}) || die "Unable to mkdir $ENV{CSCOUT_SPY_TMPDIR}: $!\n";

open(OUT, ">$ENV{CSCOUT_SPY_TMPDIR}/empty.c");
close(OUT);
$ENV{PATH} = "$ENV{CSCOUT_SPY_TMPDIR}:$ENV{PATH}";

spy('gcc');
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
