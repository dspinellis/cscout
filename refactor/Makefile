#
# (C) Copyright 2001 Diomidis Spinellis
#
# $Id: Makefile,v 1.1 2001/08/26 05:23:29 dds Exp $
#

OBJ=eclass.o error.o fchar.o fileid.o pdtoken.o pltoken.o \
ptoken.o tchar.o token.o tokid.o tokname.o
CC=g++
CPPFLAGS=-g

all: color
#all: simple_cpp

simple_cpp: simple_cpp.o $(OBJ)
	$(CC) $(CPPFLAGS) -o simple_cpp $(OBJ) simple_cpp.o

color: color.o $(OBJ)
	$(CC) $(CPPFLAGS) -o color $(OBJ) color.o

# Test
# diff --ignore-all-space --ignore-blank-lines --ignore-matching='^# [0-9]'
