#
# (C) Copyright 2001-2005 Diomidis Spinellis
#
# $Id: Makefile,v 1.114 2007/05/07 14:01:23 dds Exp $
#

# By default a production, unsupported build is made.
# For a commercial build run make as:
# make LICENSEE="John Smith"
# For a debug build run make as:
# make DEBUG=1

# Common object files
OBJBASE=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o license.o des.o version.o \
getopt.o error.o fdep.o fcall.o call.o idquery.o query.o funquery.o \
logo.o workdb.o obfuscate.o sql.o base64.o md5.o os.o pager.o \
option.o

# monitor.o

# For miniversion
BRANCH=$(shell sed -n '/Revision:/{;s/^.*: //;s/ .*//;p;}' version.cpp)
DATE=$(shell date +'%Y/%m/%d %H:%M:%S')

# Determine architecture and OS
ARCH=$(shell ( [ `uname -p` != unknown ] && uname -p 2>/dev/null ) || uname -m )
OS=$(shell uname -s)

# Sanitize processor types
ifeq ($(ARCH),x86)
ARCH=i386
endif
ifeq ($(ARCH),i686)
ARCH=i386
endif

# Build Mach-O fat binaries
ifeq ($(OS),Darwin)
ARCH=macho
CPPFLAGS+=-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc
LDFLAGS+=-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
endif

OBJ=$(addprefix $(ARCH)/, $(OBJBASE))

.SUFFIXES:.java .dot .ps .gif .pic .eps .png .svg

# Do not add version.cpp here
SRC=Makefile attr.cpp attr.h cpp.h cscout.cpp ctoken.cpp \
	ctoken.h debug.cpp debug.h defs.h des.c des.h eclass.cpp \
	eclass.h error.cpp error.h eval.y fchar.cpp \
	fchar.h fileid.cpp fileid.h id.h incs.h license.c license.h \
	macro.cpp macro.h metrics.cpp metrics.h mscdefs.h \
	mscincs.h obfuscate.cpp parse.y pdtoken.cpp \
	pdtoken.h pltoken.cpp pltoken.h ptoken.cpp ptoken.h regex.h \
	simple_cpp.cpp stab.cpp stab.h swill.h tchar.cpp tchar.h \
	tclfuns.cpp token.cpp token.h tokid.cpp \
	tokid.h tokmap.cpp tokmap.h tokname.pl type.cpp type.h \
	type2.h wdefs.h wincs.h workdb.cpp ytab.h ytoken.h \
	version.h getopt.c getopt.h fdep.cpp \
	fdep.h fcall.cpp fcall.h mcall.h call.cpp call.h idquery.cpp idquery.h \
	query.cpp query.h funquery.cpp funquery.h logo.cpp logo.h ytoken.h \
	workdb.h obfuscate.h dlstubs.c sql.cpp sql.h fifstream.h compiledre.h \
	base64.h base64.c gdisplay.h eval.h md5.h md5.c os.cpp os.h \
	pager.h pager.cpp option.cpp option.h \
	runtest.sh \
	csmake.pl cswc.pl

ALLSRC=$(SRC) logo.png version.cpp

YACC=btyacc
#YACC=yacc
CC?=gcc
CXX?=g++

CPPFLAGS+=-pipe -Wall
CPPFLAGS+=-DSPINELLIS_SERVER
ifdef DEBUG
# Debug build
# To get yacc debugging info set YYDEBUG environment variable to 1
# To get stack traces for STL problems use gdb and break _Error_formatter::_M_at
CPPFLAGS+=-g -DCOMMERCIAL -DYYDEBUG -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
# -D_GLIBCXX_CONCEPT_CHECKS
dummy := $(shell gcc -o ccode -DCCODE des.c)
dummy := $(shell ./ccode "(internal use only)")
else
CPPFLAGS+=-DPRODUCTION -O3
endif

# Test coverage analysis
# Buld with make DEBUG=1 TCOV=1
# Test with sh runtest.sh
# Then run gcov token.cpp -o i386
ifdef TCOV
CPPFLAGS+=-ftest-coverage -fprofile-arcs
endif

# Local development flag options
#CPPFLAGS+=-DTEST_LICENSING
#CPPFLAGS+=-DNO_LOCK
#CPPFLAGS+=-DOFFICE_SERVER

# Moving away from static linking
#CPPFLAGS += -static

ifeq ($(OS),SunOS)
ADDLIBS += -lsocket -lnsl
#OBJ += dlstubs.o
endif

ifeq ($(OS),CYGWIN_NT-5.1)
CPPFLAGS += -I.
endif

ifdef LICENSEE
CPPFLAGS += -DCOMMERCIAL=1
ifeq ($(ARCH),sparc)
CPPFLAGS += -mcpu=v8
endif
dummy := $(shell gcc -o ccode -DCCODE des.c)
dummy := $(shell ./ccode "$(LICENSEE)")
endif

CPPFLAGS+=$(EXTRA_CPPFLAGS)

# Pattern rules for C and C++ files
$(ARCH)/%.o: %.c
	$(CC) -c $(CPPFLAGS) -o $@ $<

$(ARCH)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

ifeq ($(OS),CYGWIN_NT-5.1)
# Windows
all: $(ARCH)/cscout.exe
SSH=plink
SCP=pscp
FREEFALL=freefall
else
# Unix
all: $(ARCH)/cscout
SSH=ssh
SCP=scp
FREEFALL=freefall.freebsd.org
endif



SF=shell.cf.sourceforge.net

#all: simple_cpp

$(ARCH)/cscout: $(ARCH)/cscout.o  $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $(ARCH)/cscout  $(OBJ) $(ARCH)/cscout.o -L${HOME}/lib/$(ARCH) -lswill $(ADDLIBS)

# CCmalloc version
$(ARCH)/cscout-cc: $(ARCH)/cscout.o  $(OBJ)
	ccmalloc $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $(ARCH)/cscout-cc  $(OBJ) $(ARCH)/cscout.o -L${HOME}/lib/$(ARCH) -lswill

$(ARCH)/cscout.exe: $(ARCH)/cscout.o  $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $(ARCH)/cscout  $(OBJ) $(ARCH)/cscout.o -L. -lswill -lws2_32 -lregex

ccheck: license.c license.h des.c des.h
	gcc -o ccheck -DSERVER -DPRODUCTION license.c des.c

ccheck-install: ccheck
	cp ccheck /home/dds/web/istlab-cgi-bin

# Some important dependencies
error.o: pltoken.h
pdtoken.o: pltoken.h
simple_cpp.o: pltoken.h

eval.cpp: eval.y
	$(YACC) -b eval -p eval_ eval.y
	mv -f eval_tab.c eval.cpp

parse.cpp parse_tab.h: parse.y ytoken.h
	$(YACC) -d -b parse -p parse_ parse.y
	mv -f parse_tab.c parse.cpp

ytab.h: parse_tab.h
	grep 'define ' parse_tab.h >ytab.h

tokname.cpp: ytab.h tokname.pl
	perl tokname.pl ytab.h tokname.cpp

$(ARCH)/cscout.o: ytab.h
$(ARCH)/ctoken.o: ctoken.cpp ytab.h

$(ARCH)/fchar.o: fchar.cpp parse_tab.h


# Create error message documentation
mkerr:
	perl mkerr.pl >../doc/error.xml

# Create version info
version:
	co -l version.cpp
	echo '#include <string>' >version.cpp
	echo 'using namespace std;' >>version.cpp
	echo '#include "version.h"' >>version.cpp
	echo 'char Version::revision[] = "$$Revision: 1.114 $$";' >>version.cpp
	echo 'char Version::date[] = "$$Date: 2007/05/07 14:01:23 $$";' >>version.cpp
	echo "char *Version::ident[] = {" >>version.cpp
	ident $(SRC) | sed -n -f version.sed >>version.cpp
	echo "0 };" >>version.cpp
	ci -m"New version" -u version.cpp

miniversion:
	attrib -r version.cpp
	echo '#include <string>' >version.cpp
	echo 'using namespace std;' >>version.cpp
	echo '#include "version.h"' >>version.cpp
	echo 'char Version::revision[] = "$$R''evision: '$(BRANCH)' $$";' >>version.cpp
	echo 'char Version::date[] = "$$D''ate: '$(DATE)' $$";' >>version.cpp
	echo "char *Version::ident[] = {" >>version.cpp
	ident $(SRC) | sed -n -f version.sed >>version.cpp
	echo "0 };" >>version.cpp

versioncheck:
	ident $(SRC) | sed -n -f version.sed | grep '^"' | perl -p -e 's/\r//' >thisversion
	grep '^"' version.cpp >oldversion
	diff oldversion thisversion

test:
	sh runtest.sh

# Distribution
rdist-gemini:
	tar cvf - $(ALLSRC) | gzip -c | $(SSH) gemini tar -C src/cscout -xzvf -

rdist-icarian:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) icarian tar -C src/cscout -xzvf -

rdist-macmini:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) macmini tar -C src/cscout -xzvf -

rdist-freefall:
	$(SSH) $(FREEFALL) "mkdir src/cscout ; chmod 700 src/cscout"
	tar -cvf - $(ALLSRC) | gzip -c | $(SSH) $(FREEFALL) tar -C src/cscout -xzvf -
	$(SSH) $(FREEFALL) "(cd src/cscout ; gmake lf; gmake eval.cpp parse.cpp)"

rdist-sourceforge:
	$(SSH) $(SF) "mkdir src/cs ; chmod 700 src/cs"
	tar cf - $(ALLSRC) | gzip -c | $(SSH) $(SF) tar -C src/cs -xzvf -

rdist-sense:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) sense '(cd src/cscout ; gzip -dc | tar -xvf -)'
	$(SSH) sense "(cd src/cscout ; /usr/sfw/bin/gmake lf)"

rdist-niagara:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) niagara '(cd src/cscout ; gzip -dc | tar -xvf -)'
	$(SSH) niagara "(cd src/cscout ; /usr/sfw/bin/gmake lf)"

rdist-titan:
	tar -cvf - $(ALLSRC) | gzip -c | $(SSH) titan tar -C src/cscout -xzvf -
	$(SSH) titan "(cd src/cscout ; gmake lf)"

rdist-tar:
	tar cvf - $(ALLSRC) | gzip -c >cscout.tar.gz

rdist-spy:
	plink $(HOST) "perl -p -e 's/\r//' >bin/spy-make" <spy-make.pl \; \
		chmod +x bin/spy-make

lf:
	chmod +w $(ALLSRC)
	perl -pi -e 's/\r//' $(ALLSRC)

mkdir:
	-mkdir alpha  i386  sparc64  amd64  ia64 sparc

rdist-spiti-server:
	pscp Makefile des.[ch] license.[ch] spiti:src/cscout-server

rdist-istlab-server:
	pscp Makefile des.[ch] license.[ch] istlab:src/cscout-server

istlab-server:
	cc -o ${HOME}/web/istlab-cgi-bin/ccheck -DSERVER des.c license.c

spiti-server:
	cc -o /usr/local/www/cgi-bin/ccheck -DSERVER des.c license.c

# Used for regenerating the logo
Xlogo.cpp: logo.png
	echo '#include <stdio.h>' >logo.cpp
	echo '#include "logo.h"' >>logo.cpp
	echo 'const char Logo::logo_bytes[] = {' >>logo.cpp
	od -vb logo.png | tr -d "\r" | sed 's/^.......//;s/^ /0/;s/ /,0/g;s/$$/,/;s/^,$$//' >>logo.cpp
	echo '};' >>logo.cpp
	echo -n 'int Logo::len = sizeof(Logo::logo_bytes);' >>logo.cpp
#	wc -c logo.gif >>logo.cpp
#	echo ';' >>logo.cpp
wc:
	wc -l $(SRC)

enc-src:
	tar cvf - $(ALLSRC) | gzip -c | $(SSH) spiti encrypt-cscout >/dds/pubs/web/home/cscout/cscout-src.tar.gz

# sparc
sparc: rdist-sourceforge
	$(SSH) $(SF) "ssh sparc-solaris2 '(cd src/cs ; make mkdir lf all ; strip sparc/cscout)'"

clean:
	rm -f $(ARCH)/* parse_tab.h parse.cpp ytab.h tokname.cpp eval.cpp

.java.dot:
	javadoc -docletpath /dds/src/research/umlgraph/lib/UmlGraph.jar -doclet gr.spinellis.umlgraph.doclet.UmlGraph -private $<
	mv graph.dot $@

.dot.gif:
	dot -Tgif -o$@ $<

classdiag.java: $(ALLSRC)
	grep -h ":.*public" *.h | \
	grep -v binary_func | \
	sed 's/class //;s/://;s/public //;s/{//;s/  */ /g' | \
	awk '{print "class " $$1 " extends " $$2 " {}"}' >$@

classdiag.gif: classdiag.java

# Cross building FreeBSD binaries on icarian
H=/usr/home/dds/src/fbsd6
AMD64BIN=$H/obj/amd64/amd64/$H/src/tmp/usr/bin
SPARC64BIN=$H/obj/sparc64/sparc64/$H/src/tmp/usr/bin
I386BIN=$H/obj/i386/$H/src/tmp/usr/bin

crossbuild:
	rm -rf amd64 sparc64 i386
	mkdir amd64 sparc64 i386
	gmake ARCH=amd64 CXX=$(AMD64BIN)/g++ CC=$(AMD64BIN)/gcc
	gmake ARCH=sparc64 CXX=$(SPARC64BIN)/g++ CC=$(SPARC64BIN)/gcc
	gmake ARCH=i386 CXX=$(I386BIN)/g++ CC=$(I386BIN)/gcc
