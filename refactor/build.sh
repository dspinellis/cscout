#!/bin/sh
#
# Build the binaries on all supported platforms
#
# $Id: build.sh,v 1.3 2008/07/15 18:35:03 dds Exp $
#

# Windows
make clean
make CC=gcc

# Mac OS X
# For swill edit Source/*/Makefile changing  adding -arch i386 -arch ppc to CFLAGS
plink spiti wake macmini
make rdist-macmini
plink macmini 'cd src/cscout ; make clean ; make'

# Solaris
make rdist-sense
plink sense 'cd src/cscout ; gmake clean ; gmake CC=gcc'

# Linux i386
make rdist-parrot
plink parrot 'cd src/cscout ; make clean ; make'

# Linux amd64
make rdist-ikaria
plink ikaria 'cd src/cscout ; make clean ; PATH=/home/dds/bin/:$PATH make'

# FreeBSD
# For cross-building
#
# Create a supfile with the following settings:
# *default host=cvsup3.uk.FreeBSD.org
# *default base=/home/dds/var/db
# *default prefix=/home/dds/src/fbsd6
# *default release=cvs tag=RELENG_6
# *default delete use-rel-suffix
#
# Run:
# #!/bin/sh
# for a in amd64 i386 sparc64
# do
#         export MAKEOBJDIRPREFIX=/home/dds/obj/$a
#         make toolchain TARGET_ARCH=$a  2>/home/dds/tmp/make.$a.err >/home/dds/tmp/make.$a.out
# done
# To compile Swill run
# H=/home/dds/obj
# for arch in amd64 i386 sparc64; do make clean; make ARCH=$arch CXX=$H/$arch/$arch/usr/src/tmp/usr/bin/g++ CC=$H/$arch/$arch/usr/src/tmp/usr/bin/gcc RANLIB=$H/$arch/$arch/usr/src/tmp/usr/bin/ranlib AR=$H/$arch/$arch/usr/src/tmp/usr/bin/ar;cp libswill.a $HOME/lib/$arch; done

make rdist-istlab
plink istlab 'cd src/cscout ; gmake clean ; rm */* ; gmake crossbuild'
