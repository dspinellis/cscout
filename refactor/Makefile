#
# (C) Copyright 2001-2009 Diomidis Spinellis All rights reserved.
#
# You may only use this code if you agree to the terms of the CScout
# Source Code License agreement (see License.txt).
# If you do not agree to the terms, do not use the code.
#
# $Id: Makefile,v 1.174 2015/06/06 23:41:28 dds Exp $
#

# By default a production build is made.
# For a debug build run make as:
# make DEBUG=1

WEBHOME=$(UH)/dds/pubs/web/home/cscout/

CC=gcc
CXX=g++

# Common object files
OBJBASE=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o version.o \
getopt.o error.o fdep.o fcall.o call.o idquery.o query.o funquery.o \
logo.o workdb.o obfuscate.o sql.o base64.o md5.o os.o pager.o \
option.o filequery.o mcall.o filemetrics.o funmetrics.o ctconst.o \
dirbrowse.o html.o fileutils.o gdisplay.o globobj.o ctag.o timer.o

# monitor.o

ifdef PICO_QL
OBJBASE += pico_ql_search.o pico_ql_vt.o pico_ql_interface.o \
 pico_ql_search_helper.o pico_ql_test.o sqlite3.o \
 pico_ql_logo.o pico_ql_error_page.o
endif


# For miniversion
BRANCH=$(shell sed -n '/Revision:/{;s/^.*: //;s/ .*//;p;}' version.cpp)
DATE=$(shell date +'%Y/%m/%d %H:%M:%S')

# Determine architecture and OS
OS=$(shell uname -s)

OBJ=$(addprefix build/, $(OBJBASE))

.SUFFIXES:.java .dot .ps .png .pic .eps .png .svg

# Source we distribute
# Do not add Makefile version.cpp here
DISTSRC=License.txt attr.cpp attr.h cpp.h cscout.cpp ctoken.cpp \
	ctoken.h debug.cpp debug.h defs.h eclass.cpp \
	eclass.h error.cpp error.h eval.y fchar.cpp \
	fchar.h fileid.cpp fileid.h id.h incs.h \
	macro.cpp macro.h metrics.cpp metrics.h mscdefs.h \
	mscincs.h obfuscate.cpp parse.y pdtoken.cpp \
	pdtoken.h pltoken.cpp pltoken.h ptoken.cpp ptoken.h \
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
	ctag.cpp ctag.h timer.cpp timer.h \
	runtest.sh \
	csmake.pl cswc.pl style.css

# Sources needed for building
SRC=Makefile $(DISTSRC)

ALLSRC=$(SRC) logo.png version.cpp

YACC=/c/dds/src/port/btyacc/btyacc
#YACC=yacc

CPPFLAGS+=-pipe -Wall -I.
CPPFLAGS+=-DSPINELLIS_SERVER
ifdef DEBUG
# Debug build
# To get yacc debugging info set YYDEBUG environment variable to 1
# To get stack traces for STL problems use gdb and break _Error_formatter::_M_at
CPPFLAGS+=-g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -DDEBUG
# -D_GLIBCXX_CONCEPT_CHECKS
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
# Will always call the licensing server
#CPPFLAGS+=-DOFFICE_SERVER

# Moving away from static linking
#CPPFLAGS += -static

ifeq ($(OS),SunOS)
ADDLIBS += -lsocket -lnsl
#OBJ += dlstubs.o
endif

ifeq ($(OS),Windows_NT)
CPPFLAGS += -I.
endif

ifeq (build,sparc)
CPPFLAGS += -mcpu=v8
endif

ifdef PICO_QL
CPPFLAGS += -DPICO_QL -DPICO_QL_SINGLE_THREADED -I../sqlite-amalgamation


ifeq ($(OS),Windows_NT)
CPPFLAGS += -I/vol/boost_1_44_0
else
CPPFLAGS += -I../boost_1_44_0
LDFLAGS += -ldl -lpthread
endif

ALLSRC += pico_ql_erb_templates pico_ql_generator.rb picoschema.pl \
  cscout-data.sql \
  sqlite3.c sqlite3.h pico_ql_error_page.c pico_ql_interface.c \
  pico_ql_interface.h pico_ql_logo.c \
  pico_ql_search.h pico_ql_search_helper.cpp pico_ql_search_helper.h \
  pico_ql_swill_access_func.h pico_ql_test.c pico_ql_test.h pico_ql_vt.c \
  pico_ql_vt.h
endif

CPPFLAGS+=$(EXTRA_CPPFLAGS)

# Pattern rules for C and C++ files
build/%.o: %.c
	$(CC) -c $(CPPFLAGS) -o $@ $<

build/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

ifeq ($(OS),Windows_NT)
# Windows
all: build/cscout.exe
SSH=plink
SCP=pscp
FREEFALL=freefall
else
# Unix
all: build/cscout
SSH=ssh
SCP=scp
FREEFALL=freefall.freebsd.org
endif



SF=shell.cf.sourceforge.net

#all: simple_cpp

build/cscout: build/cscout.o  $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o build/cscout  $(OBJ) build/cscout.o -L${HOME}/lib/build -lswill $(ADDLIBS)

# CCmalloc version
build/cscout-cc: build/cscout.o  $(OBJ)
	ccmalloc $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o build/cscout-cc  $(OBJ) build/cscout.o -L${HOME}/lib/build -lswill

build/cscout.exe: build/cscout.o  $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o build/cscout  $(OBJ) build/cscout.o -L. -lswill -lws2_32

eval.cpp: eval.y
	$(YACC) -b eval -p eval_ eval.y
	mv -f eval.tab.c eval.cpp

parse.cpp parse.tab.h: parse.y ytoken.h
	@echo Expect 3 shift/reduce conflicts
	$(YACC) -d -b parse -p parse_ parse.y
	mv -f parse.tab.c parse.cpp

ytab.h: parse.tab.h
	grep 'define ' parse.tab.h >ytab.h

build/cscout.o: cscout.cpp ytab.h

tokname.cpp: ytab.h tokname.pl
	perl tokname.pl ytab.h tokname.cpp

build/html.o: html.cpp css.c

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
	echo 'char Version::revision[] = "$$Revision: 1.174 $$";' >>version.cpp
	echo 'char Version::date[] = "$$Date: 2015/06/06 23:41:28 $$";' >>version.cpp
	echo "const char *Version::ident[] = {" >>version.cpp
	ident $(SRC) | sed -n -f version.sed >>version.cpp
	echo "0 };" >>version.cpp
	ci -m"New version" -u version.cpp
	-rcs -nR`ident version.cpp | gawk '/Revision/{print $$2}' | sed 's/\./-/g'`: $(SRC)
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

rdist-gemini:
	tar cvf - $(ALLSRC) | gzip -c | $(SSH) aegean.dmst.aueb.gr ssh gemini tar -C src/cscout -xzvf -

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
	$(SSH) ikaria "(cd src/cscout ; make lf)"

rdist-tar:
	tar cvf - $(ALLSRC) | gzip -c >cscout.tar.gz

rdist-spy:
	plink $(HOST) "perl -p -e 's/\r//' >bin/spy-make" <csmake.pl \; \
		chmod +x bin/spy-make

lf:
	chmod +w $(ALLSRC)
	perl -pi -e 's/\r//' $(ALLSRC)
	rm -f regex.h

mkdir:
	-mkdir alpha  i386  sparc64  amd64  ia64 sparc

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
	rm -f build/* parse.tab.h parse.cpp ytab.h tokname.cpp eval.cpp

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

ifdef PICO_QL
pico_ql_search.cpp: pico_ql_generator.rb cscout-data.sql
	perl picoschema.pl cscout-data.sql >cscout-data.auto
	ruby pico_ql_generator.rb cscout-data.auto

pico_ql_search.o: pico_ql_search.cpp pico_ql_search.h pico_ql_interface.h pico_ql_search_helper.h

pico_ql_vt.o: pico_ql_vt.c pico_ql_vt.h pico_ql_search.h
pico_ql_interface.o: pico_ql_interface.c pico_ql_interface.h pico_ql_test.h
pico_ql_search_helper.o: pico_ql_search_helper.cpp pico_ql_search_helper.h pico_ql_search.h
pico_ql_test.o: pico_ql_test.c pico_ql_test.h
pico_ql_logo.o: pico_ql_logo.c pico_ql_swill_access_func.h
pico_ql_error_page.o: pico_ql_error_page.c pico_ql_swill_access_func.h


sqlite3.o: sqlite3.o

endif
