#
# (C) Copyright 2001-2016 Diomidis Spinellis
#
# This file is part of CScout.
#
# CScout is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CScout is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CScout.  If not, see <http://www.gnu.org/licenses/>.
#

# By default a production build is made.
# For a debug build run make as:
# make DEBUG=1

WEBHOME=$(UH)/dds/pubs/web/home/cscout/

.PHONY: test

CC=gcc
CXX=g++
ADDLIBS+=../swill/libswill.a
CXXFLAGS+=-std=c++11

# Common object files
OBJBASE=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o version.o \
error.o fdep.o fcall.o call.o idquery.o query.o funquery.o \
logo.o workdb.o obfuscate.o sql.o md5.o os.o pager.o \
option.o filequery.o mcall.o filemetrics.o funmetrics.o ctconst.o \
dirbrowse.o html.o fileutils.o gdisplay.o globobj.o ctag.o timer.o

# monitor.o

ifdef PICO_QL
OBJBASE += pico_ql_search.o pico_ql_vt.o pico_ql_interface.o \
 pico_ql_search_helper.o pico_ql_test.o sqlite3.o \
 pico_ql_logo.o pico_ql_error_page.o
endif

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
	version.h fdep.cpp \
	fdep.h fcall.cpp fcall.h mcall.h call.cpp call.h idquery.cpp idquery.h \
	query.cpp query.h funquery.cpp funquery.h logo.cpp logo.h ytoken.h \
	workdb.h obfuscate.h dlstubs.c sql.cpp sql.h fifstream.h compiledre.h \
	gdisplay.h eval.h md5.h md5.c os.cpp os.h \
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

YACC=../btyacc/btyacc
#YACC=yacc

CPPFLAGS+=-pipe -Wall -I.
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


#all: simple_cpp

build/cscout: build/cscout.o $(OBJ)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o build/cscout  $(OBJ) build/cscout.o $(ADDLIBS)

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

ifneq ($(shell sed -n '/Version::revision/s/.* = "\(.*\)";/\1/p' version.cpp), $(shell git log -n 1 --format='%H'))
version.cpp:
	echo '#include <string>' >version.cpp
	echo 'using namespace std;' >>version.cpp
	echo '#include "version.h"' >>version.cpp
	echo 'char Version::revision[] = "'$$(git log -n 1 --format='%H')'";' >>version.cpp
	echo 'char Version::date[] = "'$$(git log -n 1 --format='%ci')'";' >>version.cpp
	echo 'char Version::compiled[] = "'$$(date +'%F %T')'";' >>version.cpp

.PHONY: version.cpp
endif


test:
	./runtest.sh $(TEST_FLAGS)

# Used for regenerating the logo
logo.cpp: logo.png
	echo '#include <stdio.h>' >logo.cpp
	echo '#include "logo.h"' >>logo.cpp
	echo 'const unsigned char Logo::logo_bytes[] = {' >>logo.cpp
	od -vb logo.png | tr -d "\r" | sed 's/^.......//;s/^ /0/;s/ /,0/g;s/$$/,/;s/^,$$//' >>logo.cpp
	echo '};' >>logo.cpp
	echo -n 'int Logo::len = sizeof(Logo::logo_bytes);' >>logo.cpp

wc:
	wc -l $(SRC)

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
