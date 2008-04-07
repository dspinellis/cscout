#!/bin/sh
#
# Release a CScout version
# a -c flag will include a remote copy step
#
# $Id: release.sh,v 1.5 2008/04/07 20:26:20 dds Exp $
#

DESTDIR="${UH}/dds/pubs/web/home/cscout"
#DESTDIR='c:/tmp/cscout-try'
VERSION=`sed -n '/Revision:/{;s/^.*: //;s/ .*//;p;}' refactor/version.cpp`
DISTDIR=cscout-$VERSION

# Finish the execution with an error
die()
{
	echo "$1 failed" 1>&1
	exit 1
}

# Copy a binary file into the local distribution
copyfile()
{
	INFILE=$1
	DIST=$2
	mkdir -p bin/$DIST/$DISTDIR/bin
	pscp $INFILE bin/$DIST/$DISTDIR/bin/cscout || die "copy $INFILE to bin/$DIST/$DISTDIR/bin/cscout"
}

if [ "$1" == "-c" ]
then
	# Copy remote files
	rm -rf bin

	# Windows
	mkdir -p bin/win32-i386/$DISTDIR/bin
	cp refactor/i386/cscout.exe bin/win32-i386/$DISTDIR/bin/cscout.exe || die "copy $INFILE to bin/win32-i386/$DISTDIR/bin/cscout.exe"
	# Mac OS X
	plink spiti wakemac
	copyfile macmini:src/cscout/macho/cscout darwin-macho
	# Linux
	copyfile parrot:src/cscout/i386/cscout linux-i386
	copyfile titan:src/cscout/x86_64/cscout linux-x86_64
	# Solaris
	copyfile sense:src/cscout/sparc/cscout solaris-sparc
	# FreeBSD
	for arch in alpha amd64 i386 ia64 powerpc sparc64
	do
		copyfile istlab:src/cscout/$arch/cscout fbsd-$arch
	done
fi


# Create the neutral directory
cmd /c rd /q/s $DISTDIR
mkdir $DISTDIR || die "Creating $DISTDIR"
tar -cf - -C /dds/pubs/web/home/cscout doc --exclude=RCS \
	-C /dds/src/research/cscout README example etc include man |
tar -xf - -C $DISTDIR  || die "Creating the neutral directory"

# Create the neutral zip file
NEUTRAL_ZIP=${DESTDIR}/cscout-${VERSION}-neutral.zip
rm -f ${NEUTRAL_ZIP}
zip -r ${NEUTRAL_ZIP} $DISTDIR || die "zip $DISTDIR into $NEUTRAL_ZIP"

# Create the neutral tar file
# Remove (in-place) carriage returns
perl dirlf.pl $DISTDIR
# tar it
tar -cvf - $DISTDIR |
gzip -c >${DESTDIR}/cscout-${VERSION}-neutral.tar.gz || die 'tar neutral'

# Remove carriage returns from the specified file
tolf()
{
	perl -p -e 'BEGIN {binmode(STDOUT);} s/\r//;' $1 >$2 || die "Converting $1 to lf format as $2"
}

# Convert the specified file into a batch file
tobatch()
{
	SRC=$1
	DST=$2
	echo "@REM=('" >$DST || die "Create $DST"
	echo "@perl /usr/local/bin/%0.bat %1 %2 %3 %4 %5 %6 %7 %8 %9" >>$DST
	echo "@goto end ') if 0 ;" >>$DST
	cat $SRC >>$DST || die "Append $SRC to $DST"
	echo "@REM=('" >>$DST
	echo ":end ') if 0 ;" >>$DST
}

# Create a Unix binary file
binfile()
{
	DIST=$1
	tolf refactor/cswc.pl bin/$DIST/$DISTDIR/bin/cswc
	tolf refactor/csmake.pl bin/$DIST/$DISTDIR/bin/csmake
	tar -cvf - -C bin/$DIST $DISTDIR/bin --mode=+x |
	gzip -c >${DESTDIR}/cscout-${VERSION}-$DIST.tar.gz || die 'Creating tar binary package'
}


# Create the Unix binary files
rm -f bin/win32-i386/$DISTDIR/bin/cswc.bat bin/win32-i386/$DISTDIR/bin/csmake.bat
for i in win32-i386 darwin-macho linux-i386 linux-x86_64 solaris-sparc fbsd-i386 fbsd-amd64 fbsd-sparc64
do
	binfile $i
done

# Create the Windows zip file
BIN_ZIP=${DESTDIR}/cscout-${VERSION}-win32-i386.zip
rm -f ${BIN_ZIP}
rm -f bin/win32-i386/$DISTDIR/bin/cswc bin/win32-i386/$DISTDIR/bin/csmake
tobatch refactor/cswc.pl bin/win32-i386/$DISTDIR/bin/cswc.bat
tobatch refactor/csmake.pl bin/win32-i386/$DISTDIR/bin/csmake.bat

(cd bin/win32-i386 ; zip -r ${BIN_ZIP} $DISTDIR || die "zip $DISTDIR into $BIN_ZIP")

# Create the HTML index file
sed -e "s/VERSION/${VERSION}/" doc/index.html >${DESTDIR}/index.html
