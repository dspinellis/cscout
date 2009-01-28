#
# (C) Copyright 2001-2009 Diomidis Spinellis All rights reserved.
#
# You may only use this code if you agree to the terms of the CScout
# Source Code License agreement (see License.txt).
# If you do not agree to the terms, do not use the code.
#
# $Id: Makefile,v 1.153 2009/01/28 11:17:02 dds Exp $
#

# By default a production, unsupported build is made.
# For a commercial build run make as:
# make LICENSEE="John Smith"
# For a debug build run make as:
# make DEBUG=1

WEBHOME=$(UH)/dds/pubs/web/home/cscout/

CC=gcc
CXX=g++

# Common object files
OBJBASE=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o license.o des.o version.o \
getopt.o error.o fdep.o fcall.o call.o idquery.o query.o funquery.o \
logo.o workdb.o obfuscate.o sql.o base64.o md5.o os.o pager.o \
option.o filequery.o mcall.o filemetrics.o funmetrics.o ctconst.o \
dirbrowse.o html.o fileutils.o gdisplay.o globobj.o

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
CC=gcc-4.2
CXX=g++-4.2
ARCH=macho
CPPFLAGS+=-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch i386 -arch ppc
LDFLAGS+=-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
endif

OBJ=$(addprefix $(ARCH)/, $(OBJBASE))

.SUFFIXES:.java .dot .ps .png .pic .eps .png .svg

# Source we distribute
# Do not add Makefile version.cpp license.c license.h here
DISTSRC=License.txt attr.cpp attr.h cpp.h cscout.cpp ctoken.cpp \
	ctoken.h debug.cpp debug.h defs.h des.c des.h eclass.cpp \
	eclass.h error.cpp error.h eval.y fchar.cpp \
	fchar.h fileid.cpp fileid.h id.h incs.h \
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
	filequery.cpp filequery.h mquery.h mcall.cpp  filemetrics.h funmetrics.h \
	pager.h pager.cpp option.cpp option.h  filemetrics.cpp funmetrics.cpp \
	ctconst.cpp ctconst.h gdisplay.cpp  globobj.cpp globobj.h \
	html.h html.cpp dirbrowse.h dirbrowse.cpp fileutils.cpp fileutils.h \
	runtest.sh \
	csmake.pl cswc.pl style.css

# Sources needed for building
SRC=Makefile $(DISTSRC) license.c license.h

ALLSRC=$(SRC) logo.png version.cpp

YACC=btyacc
#YACC=yacc

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
# Displays information on licensing protocol
#CPPFLAGS+=-DDEBUG_LICENSING
# Will always call the licensing server
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
	cp ccheck /home/dds/web/spinellis-cgi-bin/

eval.cpp: eval.y
	$(YACC) -b eval -p eval_ eval.y
	mv -f eval_tab.c eval.cpp

parse.cpp parse_tab.h: parse.y ytoken.h
	@echo Expect 3 shift/reduce conflicts
	$(YACC) -d -b parse -p parse_ parse.y
	mv -f parse_tab.c parse.cpp

ytab.h: parse_tab.h
	grep 'define ' parse_tab.h >ytab.h

$(ARCH)/cscout.o: cscout.cpp ytab.h

tokname.cpp: ytab.h tokname.pl
	perl tokname.pl ytab.h tokname.cpp

$(ARCH)/html.o: html.cpp css.c

css.c: style.css
	tr -d \\r <$? | sed 's/\\/\\\\/g;s/"/\\"/g;s/^/"/;s/$$/\\n"/' >$@

# Create error message documentation
mkerr:
	perl mkerr.pl >../doc/error.xml

# Create version info
version:
	co -l version.cpp
	echo '#include <string>' >version.cpp
	echo 'using namespace std;' >>version.cpp
	echo '#include "version.h"' >>version.cpp
	echo 'char Version::revision[] = "$$Revision: 1.153 $$";' >>version.cpp
	echo 'char Version::date[] = "$$Date: 2009/01/28 11:17:02 $$";' >>version.cpp
	echo "const char *Version::ident[] = {" >>version.cpp
	ident $(SRC) | sed -n -f version.sed >>version.cpp
	echo "0 };" >>version.cpp
	ci -m"New version" -u version.cpp
	rcs -nR`ident version.cpp | gawk '/Revision/{print $$2}' | sed 's/\./-/g'`: $(SRC)
	echo Manually fix ../doc/change.xml

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
rdist-parrot:
	tar cvf - $(ALLSRC) | gzip -c | $(SSH) parrot tar -C src/cscout -xzvf -

rdist-icarian:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) icarian tar -C src/cscout -xzvf -

rdist-macmini:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) macmini tar -C src/cscout -xzvf -

rdist-istlab:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) istlab tar -C src/cscout -xzvf -

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

rdist-test-sense:
	tar cf - test | gzip -c | $(SSH) sense '(cd src/cscout ; gzip -dc | tar -xvf -)'
	tar -C .. -cf - example example.obf | gzip -c | $(SSH) sense '(cd src ; gzip -dc | tar -xvf -)'

rdist-niagara:
	tar cf - $(ALLSRC) | gzip -c | $(SSH) niagara '(cd src/cscout ; gzip -dc | tar -xvf -)'
	$(SSH) niagara "(cd src/cscout ; /usr/sfw/bin/gmake lf)"

rdist-titan:
	tar -cvf - $(ALLSRC) | gzip -c | $(SSH) titan tar -C src/cscout -xzvf -
	$(SSH) titan "(cd src/cscout ; gmake lf)"

rdist-ikaria:
	tar -cvf - $(ALLSRC) | gzip -c | $(SSH) ikaria tar -C src/cscout -xzvf -
	$(SSH) ikaria "(cd src/cscout ; gmake lf)"

rdist-tar:
	tar cvf - $(ALLSRC) | gzip -c >cscout.tar.gz

rdist-spy:
	plink $(HOST) "perl -p -e 's/\r//' >bin/spy-make" <csmake.pl \; \
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
	tar cvf - $(ALLSRC) | gzip -c | $(SSH) spiti encrypt-cscout >$(UH)/dds/pubs/web/home/cscout/cscout-src.tar.gz

# sparc
sparc: rdist-sourceforge
	$(SSH) $(SF) "ssh sparc-solaris2 '(cd src/cs ; make mkdir lf all ; strip sparc/cscout)'"

clean:
	rm -f $(ARCH)/* parse_tab.h parse.cpp ytab.h tokname.cpp eval.cpp

.java.dot:
	cmd /c javadoc.exe -docletpath /dds/src/research/umlgraph/lib/UmlGraph.jar -doclet org.umlgraph.doclet.UmlGraph -private $<
	mv graph.dot $@

.dot.png:
	dot -Tpng -o$@ $<

classdiag.java: $(ALLSRC)
	grep -h ":.*public" *.h | \
	egrep -v '(struct|binary_func)' | \
	sed 's/class //;s/://;s/public //;s/{//;s/  */ /g' | \
	awk '{print "class " $$1 " extends " $$2 " {}"}' >$@

classdiag.png: classdiag.java

# Cross building FreeBSD binaries on istlab
# Cross-build the toolchain by running: make-toolchain
H=/usr/home/dds/obj
ALLARCH=amd64 i386 sparc64

crossbuild:
	for arch in $(ALLARCH);\
	do \
		mkdir $$arch 2>/dev/null ;\
		gmake ARCH=$$arch CXX=$H/$$arch/$$arch/usr/src/tmp/usr/bin/g++ CC=$H/$$arch/$$arch/usr/src/tmp/usr/bin/gcc ;\
	done

crossbuild-clean:
	for arch in $(ALLARCH);\
	do \
		rm -rf $$arch ;\
	done

dbdump:
	for db in mysql hsqldb postgres;\
	do \
		(cd ../example ; \
		../refactor/i386/cscout -s $$db awk.cs | gzip -c > $(WEBHOME)/awk-$$db.sql.gz) ;\
	done

obfuscation:
	( cd ../example.obf ; \
	sh run.sh ; \
	tar cf - awk | gzip -c > $(WEBHOME)/awk-obf.tar.gz ; \
	zip -r $(WEBHOME)/awk-obf.zip awk ; \
	)

distco:
	co -l $(DISTSRC)

distedit:
	wi $(DISTSRC)

distci:
	ci -u $(DISTSRC)

evalsrc:
	rm -rf cscout-$(BRANCH)-evalsrc
	mkdir cscout-$(BRANCH)-evalsrc
	cp $(DISTSRC) cscout-$(BRANCH)-evalsrc
	tar czvf cscout-$(BRANCH)-evalsrc.tar.gz cscout-$(BRANCH)-evalsrc
	rm -rf cscout-$(BRANCH)-evalsrc
