#
# (C) Copyright 2001 Diomidis Spinellis
#
# $Id: Makefile,v 1.6 2001/09/04 13:26:14 dds Exp $
#

OBJ=eclass.o error.o fchar.o fileid.o pdtoken.o pltoken.o debug.o \
ptoken.o tchar.o token.o tokid.o tokname.o eval.tab.o ctoken.o macro.o \
parse.o type.o

CC=g++
CPPFLAGS=-pipe -g

CPPTEST=test/trigtest.c test/color.c test/stringize.c test/simpledef.c \
test/noexpand.c test/expand.c  test/mreplace.c test/noreplace.c \
test/nested.c test/mkfun.c test/ansi-p92.c \
test/twostring.c test/parthomo.c test/ansi-p93-l5.c \
test/stresc.c  test/ansi-p93-l34.c test/if.c test/if2.c test/if3.c \
test/empty.c test/def.c test/conddef.c

#all: color
all: simple_cpp

simple_cpp: simple_cpp.o $(OBJ)
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) simple_cpp.o

color: color.o $(OBJ)
	$(CC) $(CPPFLAGS) -o color $(OBJ) color.o

# Some important dependencies
color.o: pltoken.h
error.o: pltoken.h
pdtoken.o: pltoken.h
simple_cpp.o: pltoken.h

eval.tab.c: eval.y
	$(YACC) -b eval -p eval_ eval.y

parse.cpp: parse.y
	$(YACC) -b parse -p parse_ parse.y
	mv parse.tab.c parse.cpp

# Test
cpptest: cpp-test $(CPPTEST)
	for i in $(CPPTEST) ; do echo -n $$i && cpp-test $$i && echo ' ok' ; done

hellotest: simple_cpp test/hello.c
	simple_cpp test/hello.c >hello.i
	g++ hello.i
	a.out

boottest: simple_cpp
	simple_cpp simple_cpp.cpp >simple_cpp.i
	g++ -c simple_cpp.i
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) simple_cpp.o
