#
# (C) Copyright 2001 Diomidis Spinellis
#
# $Id: Makefile,v 1.37 2003/07/07 12:27:44 dds Exp $
#

# Common object files
OBJ=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o license.o des.o version.o

# Do not add version.cpp here
SRC=Makefile attr.cpp attr.h color.cpp cpp.h cscout.cpp ctoken.cpp \
	ctoken.h debug.cpp debug.h defs.h des.c des.h eclass.cpp \
	eclass.h engine.cpp error.cpp error.h eval.y fchar.cpp \
	fchar.h fileid.cpp fileid.h id.h incs.h license.c license.h \
	macro.cpp macro.h metrics.cpp metrics.h minihttp.h mscdefs.h \
	mscincs.h obfuscate.cpp parse.y pdtoken.cpp \
	pdtoken.h pltoken.cpp pltoken.h ptoken.cpp ptoken.h regex.h \
	simple_cpp.cpp stab.cpp stab.h swill.h tchar.cpp tchar.h \
	tclfuns.cpp token.cpp token.h tokid.cpp \
	tokid.h tokmap.cpp tokmap.h tokname.pl type.cpp type.h \
	type2.h wdefs.h webmap.cpp wincs.h workdb.cpp ytab.h ytoken.h \
	prjcomp.pl version.h version.cpp

# Stand-alone object files
STANDOBJ=error.o

# Tcl object files
TCLOBJ=tclfuns.o

YACC=btyacc
#YACC=yacc
CC=g++
# AUEB development flags
#CPPFLAGS=-pipe -g -Wall -DTEST_LICENSING -DSPINELLIS_SERVER

# Local development flags
#CPPFLAGS=-pipe -g -Wall -DTEST_LICENSING -DOFFICE_SERVER

# Production flags
CPPFLAGS=-pipe -O2 -DSPINELLIS_SERVER -DPRODUCTION -static

# Realistic debug flags
#CPPFLAGS=-pipe -O2 -g -DSPINELLIS_SERVER -DPRODUCTION

CPPTEST=test/trigtest.c test/color.c test/stringize.c test/simpledef.c \
test/noexpand.c test/expand.c  test/mreplace.c test/noreplace.c \
test/nested.c test/mkfun.c test/ansi-p92.c \
test/twostring.c test/parthomo.c test/ansi-p93-l5.c \
test/stresc.c  test/ansi-p93-l34.c test/if.c test/if2.c test/if3.c \
test/empty.c test/def.c test/conddef.c test/concat-empty.c

all: cscout.exe
#all: obfuscate
#all: workdb
#all: webmap
#all: color
#all: engine
#all: simple_cpp

simple_cpp: simple_cpp.o $(OBJ) $(STANDOBJ)
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) $(STANDOBJ) simple_cpp.o

simple_cpp.exe: simple_cpp.o $(OBJ) $(STANDOBJ)
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) $(STANDOBJ) simple_cpp.o  -L. -lsill -lws2_32 -lregex

workdb: workdb.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o workdb $(STANDOBJ) $(OBJ) workdb.o

cscout: cscout.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o cscout $(STANDOBJ) $(OBJ) cscout.o -L. -lswill

cscout.exe: cscout.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o cscout $(STANDOBJ) $(OBJ) cscout.o -L. -lswill -lws2_32 -lregex

ccheck: license.c license.h des.c des.h
	gcc -o ccheck -DSERVER -DPRODUCTION license.c des.c

ccheck-install: ccheck
	cp ccheck /home/dds/web/istlab-cgi-bin

webmap: webmap.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o webmap $(STANDOBJ) $(OBJ) webmap.o

color: color.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o color $(STANDOBJ) $(OBJ) color.o

obfuscate: obfuscate.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o obfuscate $(STANDOBJ) $(OBJ) obfuscate.o

engine: engine.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o engine $(STANDOBJ) $(OBJ) engine.o



# Some important dependencies
color.o: pltoken.h
error.o: pltoken.h
pdtoken.o: pltoken.h
simple_cpp.o: pltoken.h

eval.cpp: eval.y
	$(YACC) -b eval -p eval_ eval.y
	mv -f eval_tab.c eval.cpp

parse.cpp: parse.y
	$(YACC) -b parse -p parse_ parse.y
	mv -f parse_tab.c parse.cpp

tokname.cpp: ytoken.h tokname.pl
	$(YACC) -d -b parse -p parse_ parse.y
	grep 'define ' parse_tab.h >ytab.h
	perl tokname.pl ytab.h tokname.cpp

license.o: license.c
	gcc -c $(CPPFLAGS) license.c

des.o: des.c
	gcc -c $(CPPFLAGS) des.c

# Create error message documentation
mkerr:
	perl mkerr.pl >../doc/error.xml

# Create version info
version:
	co -l version.cpp
	echo '#include <string>' >version.cpp
	echo 'using namespace std;' >>version.cpp
	echo '#include "version.h"' >>version.cpp
	echo 'char Version::revision[] = "$$Revision: 1.37 $$";' >>version.cpp
	echo 'char Version::date[] = "$$Date: 2003/07/07 12:27:44 $$";' >>version.cpp
	echo "char *Version::ident[] = {" >>version.cpp
	ident $(SRC) | sed -n -f version.sed >>version.cpp
	echo "0 };" >>version.cpp
	ci -m"New version" -u version.cpp

# Test
cpptest: cpp-test $(CPPTEST) simple_cpp
	for i in $(CPPTEST) ; do echo -n $$i && cpp-test $$i && echo ' ok' ; done

hellotest: simple_cpp test/hello.c
	simple_cpp test/hello.c >hello.i
	g++ hello.i
	a.out

boottest: simple_cpp
	simple_cpp simple_cpp.cpp >simple_cpp.i
	g++ -c simple_cpp.i
	$(CC) $(CPPFLAGS) -o simple_cpp $(STANDOBJ) $(OBJ) simple_cpp.o

# Distribution
rdist-jonathan:
	tar cvf - $(SRC) | plink jonathan tar -C src/cscout -xvf -

rdist-spiti:
	tar cvf - $(SRC) | plink spiti tar -C src/cscout -xvf -

rdist-freefall:
	tar cvf - $(SRC) | gzip -c | plink freefall tar -C src/cscout -xzvf -

rdist-istlab:
	tar cvf - $(SRC) | plink istlab tar -C src/cscout -xvf -

lf:
	chmod +w $(SRC)
	perl -pi -e 's/\r//' $(SRC)

rdist-spiti-server:
	pscp Makefile des.[ch] license.[ch] spiti:src/cscout-server

rdist-istlab-server:
	pscp Makefile des.[ch] license.[ch] istlab:src/cscout-server

istlab-server:
	cc -o ${HOME}/web/istlab-cgi-bin/ccheck -DSERVER des.c license.c

spiti-server:
	cc -o /usr/local/www/cgi-bin/ccheck -DSERVER des.c license.c
