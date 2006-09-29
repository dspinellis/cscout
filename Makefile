#
# $Id: Makefile,v 1.4 2006/09/29 09:07:33 dds Exp $
#

DESTDIR="c:/dds/pubs/web/home/cscout"
VERSION=$(shell sed -n '/Revision:/{;s/^.*: //;s/ .*//;p;}' refactor/version.cpp)
NEUTRAL-ZIP=${DESTDIR}/cscout-${VERSION}-neutral.zip

dummy:
	echo Specify the all target to rebuild all architectures and neutral files

all: html fbsd-i386-tar solaris-sparc-tar linux-i386-tar linux-x86_64-tar win32-i386-tar fbsd-sparc64-tar \
	fbsd-ia64-tar fbsd-amd64-tar neutral-tar neutral-zip \
	win32-i386-zip fbsd-alpha-tar

neutral-tar:
	tar -cvf - -C /dds/pubs/web/home cscout/doc --exclude=RCS \
	-C /dds/src/research cscout/README cscout/example \
	cscout/etc cscout/include cscout/man | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-neutral.tar.gz

neutral-zip:
	rm -f ${NEUTRAL-ZIP}
	cd /dds/pubs/web/home ; zip -r ${NEUTRAL-ZIP} cscout/doc
	cd /dds/src/research ; zip -r ${NEUTRAL-ZIP} cscout/README \
	cscout/example \
	cscout/etc cscout/include cscout/man \
	-x cscout/include/RCS/\* \
	-x cscout/man/RCS/\*

fbsd-i386-tar:
	cmd /c attrib -r bin\fbsd-i386\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/fbsd-i386/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/fbsd-i386/cscout/bin/cswc
	tar -cvf - -C bin/fbsd-i386 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-freebsd-i386.tar.gz

fbsd-sparc64-tar:
	cmd /c attrib -r bin\fbsd-sparc64\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/fbsd-sparc64/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/fbsd-sparc64/cscout/bin/cswc
	tar -cvf - -C bin/fbsd-sparc64 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-freebsd-sparc64.tar.gz

fbsd-ia64-tar:
	cmd /c attrib -r bin\fbsd-ia64\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/fbsd-ia64/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/fbsd-ia64/cscout/bin/cswc
	tar -cvf - -C bin/fbsd-ia64 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-freebsd-ia64.tar.gz

fbsd-amd64-tar:
	cmd /c attrib -r bin\fbsd-amd64\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/fbsd-amd64/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/fbsd-amd64/cscout/bin/cswc
	tar -cvf - -C bin/fbsd-amd64 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-freebsd-amd64.tar.gz

linux-i386-tar:
	cmd /c attrib -r bin\linux-i386\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/linux-i386/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/linux-i386/cscout/bin/cswc
	tar -cvf - -C bin/linux-i386 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-linux-i386.tar.gz

linux-x86_64-tar:
	cmd /c attrib -r bin\linux-x86_64\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/linux-x86_64/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/linux-x86_64/cscout/bin/cswc
	tar -cvf - -C bin/linux-x86_64 cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-linux-x86_64.tar.gz

solaris-sparc-tar:
	cmd /c attrib -r bin\solaris-sparc\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/solaris-sparc/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/solaris-sparc/cscout/bin/cswc
	tar -cvf - -C bin/solaris-sparc cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-solaris-sparc.tar.gz

cswc.bat:
	echo "@REM=('" >bin/win32-i386/cscout/bin/cswc.bat
	echo "@perl /usr/local/bin/%0.bat %1 %2 %3 %4 %5 %6 %7 %8 %9" >>bin/win32-i386/cscout/bin/cswc.bat
	echo "@goto end ') if 0 ;" >>bin/win32-i386/cscout/bin/cswc.bat
	cat refactor/prjcomp.pl >>bin/win32-i386/cscout/bin/cswc.bat
	echo "@REM=('" >>bin/win32-i386/cscout/bin/cswc.bat
	echo ":end ') if 0 ;" >>bin/win32-i386/cscout/bin/cswc.bat

win32-i386-tar: cswc.bat
	tar -cvf - -C bin/win32-i386 cscout/bin | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-win32-i386.tar.gz

win32-i386-zip: cswc.bat
	cd bin/win32-i386 ; zip -r ${DESTDIR}/cscout-${VERSION}-win32-i386.zip	cscout/bin

fbsd-alpha-tar:
	cmd /c attrib -r bin\fbsd-alpha\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/fbsd-alpha/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/fbsd-alpha/cscout/bin/cswc
	tar -cvf - -C bin/fbsd-alpha cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-freebsd-alpha.tar.gz

darwin-powerpc-tar:
	cmd /c attrib -r bin\darwin-powerpc\cscout\bin\cswc
	cp refactor/prjcomp.pl bin/darwin-powerpc/cscout/bin/cswc
	perl /usr/local/bin/lf.bat bin/darwin-powerpc/cscout/bin/cswc
	tar -cvf - -C bin/darwin-powerpc cscout/bin --mode=+x | \
	gzip -c >${DESTDIR}/cscout-${VERSION}-darwin-powerpc.tar.gz


html:
	sed -e "s/VERSION/${VERSION}/" doc/index.html >${DESTDIR}/index.html
