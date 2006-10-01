#!/usr/bin/perl
#
# Convert the files in the specified directory into Unix text format
#
# $Id: dirlf.pl,v 1.1 2006/10/01 18:04:24 dds Exp $
#

use File::Find;

find(\&wanted, $ARGV[0]);

sub wanted
{
	return unless -f;
	my $fname  = $_;
	my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat $fname;
	rename($_, $rm = "$_.bak") || die "rename $_ $_.bak";
	open(IN, "$_.bak") || die "open $_.bak for reading";
	open(OUT, ">$_") || die "open $_ for writing";
	binmode(OUT);
	while (<IN>) {
		s/\r//;
		print OUT;
	}
	close(IN);
	close(OUT);
	unlink($rm);
	utime($atime, $mtime, $fname) || die "utime($atime, $mtime, $fname): $!\n";
	chmod($mode, $fname) || die "chmod($mode, $fname: $!\n";
}
