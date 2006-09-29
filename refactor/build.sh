#!/bin/sh
#
# Build the binaries on all supported platforms
#
# $Id: build.sh,v 1.1 2006/09/29 21:04:56 dds Exp $
#

# Windows
make clean
make CC=gcc

# Mac OS X
plink spiti wakemac
make rdist-macmini
plink macmini 'cd src/cscout ; make clean ; make'

# Solaris
make rdist-sense
plink sense 'cd src/cscout ; gmake clean ; gmake CC=gcc'

# Linux i386
make rdist-gemini
plink gemini 'cd src/cscout ; make clean ; make'

# Linux amd64
make rdist-titan
plink titan 'cd src/cscout ; make clean ; make'

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
#         export MAKEOBJDIRPREFIX=/home/dds/src/fbsd6/obj/$a
#         make toolchain TARGET_ARCH=$a  2>make.$a.err >make.$a.out
# done
make rdist-icarian
plink icarian 'cd src/cscout ; gmake clean ; gmake crossbuild'
