#!/bin/sh
#
# Release a CScout version
# a -c flag will include a remote copy step
# a -w flag will only update the web page
#
# See release.txt for other instructions.
#
# $Id: release.sh,v 1.13 2011/10/24 10:36:57 dds Exp $
#

DESTDIR="/dds/pubs/web/home/cscout"
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
	echo copy $INFILE
	mkdir -p bin/$DIST/$DISTDIR/bin
	pscp $INFILE bin/$DIST/$DISTDIR/bin/cscout || die "copy $INFILE to bin/$DIST/$DISTDIR/bin/cscout"
}

# Update web page
webinst()
{
	cd web
	sh build.sh
	cp build/*.html ${UH}/${DESTDIR}
	sed -e "s/VERSION/${VERSION}/" build/download.html >${UH}/${DESTDIR}/download.html
	cd ..
}

webinst
if [ "$1" == "-w" ]
then
	exit 0
fi

if [ "$1" == "-c" ]
then
	# Copy remote files
	rm -rf bin

	# Windows
	mkdir -p bin/win32-i386/$DISTDIR/bin
	cp refactor/i386/cscout.exe bin/win32-i386/$DISTDIR/bin/cscout.exe || die "copy $INFILE to bin/win32-i386/$DISTDIR/bin/cscout.exe"
	# Linux
	#copyfile parrot:src/cscout/i386/cscout linux-i386
	plink aegean.dmst.aueb.gr scp gemini:src/cscout/i386/cscout .
	copyfile aegean.dmst.aueb.gr:cscout linux-i386
	copyfile ikaria:src/cscout/x86_64/cscout linux-x86_64
	# FreeBSD
	for arch in i386 amd64 sparc64
	do
		copyfile istlab:src/cscout/$arch/cscout fbsd-$arch
	done
	# Mac OS X
	plink spiti wake macmini
	sleep 1
	copyfile macmini:src/cscout/macho/cscout darwin-macho
	# Solaris
	copyfile sense:src/cscout/sparc/cscout solaris-sparc
fi


# Create the neutral directory
rm -rf $DISTDIR
mkdir $DISTDIR || die "Creating $DISTDIR"
tar -cf - -C ${UH}/${DESTDIR} doc --exclude=RCS \
	-C ${UH}/dds/src/research/cscout README example etc include man |
tar -xf - -C $DISTDIR  || die "Creating the neutral directory"

# Create the neutral zip file
NEUTRAL_ZIP=${DESTDIR}/cscout-${VERSION}-neutral.zip
rm -f ${UH}/${NEUTRAL_ZIP}
zip -r ${NEUTRAL_ZIP} $DISTDIR || die "zip $DISTDIR into $NEUTRAL_ZIP"

# Create the neutral tar file
# Remove (in-place) carriage returns
perl dirlf.pl $DISTDIR
# tar it
tar -cvf - $DISTDIR |
gzip -c >${UH}/${DESTDIR}/cscout-${VERSION}-neutral.tar.gz || die 'tar neutral'

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
	gzip -c >${UH}/${DESTDIR}/cscout-${VERSION}-$DIST.tar.gz || die 'Creating tar binary package'
}


# Create the Unix binary files
rm -f bin/win32-i386/$DISTDIR/bin/cswc.bat bin/win32-i386/$DISTDIR/bin/csmake.bat
for i in win32-i386 darwin-macho linux-i386 linux-x86_64 solaris-sparc \
fbsd-amd64 fbsd-i386 fbsd-sparc64

do
	binfile $i
done

# Create the Windows zip file
BIN_ZIP=${DESTDIR}/cscout-${VERSION}-win32-i386.zip
rm -f ${UH}/${BIN_ZIP}
rm -f bin/win32-i386/$DISTDIR/bin/cswc bin/win32-i386/$DISTDIR/bin/csmake
tobatch refactor/cswc.pl bin/win32-i386/$DISTDIR/bin/cswc.bat
tobatch refactor/csmake.pl bin/win32-i386/$DISTDIR/bin/csmake.bat

(cd bin/win32-i386 ; zip -r ${BIN_ZIP} $DISTDIR || die "zip $DISTDIR into $BIN_ZIP")

