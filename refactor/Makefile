#
# (C) Copyright 2001 Diomidis Spinellis
#
# $Id: Makefile,v 1.25 2002/12/26 12:46:24 dds Exp $
#

# Common object files
OBJ=eclass.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.o ctoken.o macro.o \
parse.o type.o stab.o attr.o metrics.o

# Stand-alone object files
STANDOBJ=error.o

# Tcl object files
TCLOBJ=tclfuns.o

YACC=btyacc
#YACC=yacc
CC=g++
CPPFLAGS=-pipe -g -Wall
#CPPFLAGS=-pipe -O2
TINCLUDE=/opt/ajuba/TclPro1.4/include

CPPTEST=test/trigtest.c test/color.c test/stringize.c test/simpledef.c \
test/noexpand.c test/expand.c  test/mreplace.c test/noreplace.c \
test/nested.c test/mkfun.c test/ansi-p92.c \
test/twostring.c test/parthomo.c test/ansi-p93-l5.c \
test/stresc.c  test/ansi-p93-l34.c test/if.c test/if2.c test/if3.c \
test/empty.c test/def.c test/conddef.c test/concat-empty.c

all: ceescape
#all: obfuscate
#all: workdb
#all: webmap
#all: color
#all: engine
#all: simple_cpp

simple_cpp: simple_cpp.o $(OBJ) $(STANDOBJ)
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) $(STANDOBJ) simple_cpp.o

workdb: workdb.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o workdb $(STANDOBJ) $(OBJ) workdb.o

ceescape: ceescape.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o ceescape $(STANDOBJ) $(OBJ) ceescape.o -L. -lswill

ceescape.exe: ceescape.o $(STANDOBJ) $(OBJ)
	$(CC) $(CPPFLAGS) -o ceescape $(STANDOBJ) $(OBJ) ceescape.o -L. -lswill -lws2_32

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

appinit.h:
	mktclapp -header |sed 's/cplusplus/xyzzy/' >appinit.h

tclfuns.o: tclfuns.cpp appinit.h
	$(CC) $(CFLAGS) -I$(TINCLUDE) -c tclfuns.cpp

refactor.a: $(TCLOBJ) $(OBJ)
	ar cur refactor.a $(TCLOBJ) $(OBJ)
	ranlib refactor.a
