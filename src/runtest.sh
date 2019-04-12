#!/usr/bin/env bash
#
# (C) Copyright 2001-2019 Diomidis Spinellis
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
#
# Pass -p to prime the test data and -k to continue after failed tests.
# To run only a series of tests pass one of the following options
# -TEST_AWK
# -TEST_RECONST
# -TEST_CPP
# -TEST_C
# -TEST_OBFUSCATION
#
# To run a single test set the corresponding environment variable e.g.
# CFILES=c36-endlabel.c ./runtest.sh -TEST_C
# RCFILES=c36-endlabel.c ./runtest.sh -TEST_RECONST
# CPPFILES=cpp63-rescan.c ./runtest.sh -TEST_CPP
#


echo 'TAP version 13'

if [ -r dbpoints ] && grep -q '^[a-z]' dbpoints
then
	echo "Bail out! Debug points may cause the tests to fail.  Comment or remove them."
	exit 1
fi

if ! [ "$CSCOUT_DIR" ] ; then
  if [ -r cscout.cpp ] ; then
    CSCOUT_DIR=$(pwd)/..
  else
    echo "Bail out! runtest.sh can only be run from the top-level makefile, or within src."
    exit 1
  fi
fi

if ! [ "$HSQLDB_DIR" ] ; then
  HSQLDB_DIR=$(echo $CSCOUT_DIR/hsqldb-*/hsqldb)
  if ! [ -r "$HSQLDB_DIR/lib/sqltool.jar" ] ; then
    echo "Bail out! Unable to find lib/sqltool.jar under $HSQLDB_DIR."
    exit 1
  fi
fi

# Start a test (arguments directory, name)
start_test()
{
  TEST_NAME="$2"
}

# Report a test's error output following the TAP convention
show_error()
{
  echo '#---'
  sed 's/^/#/' "$1"
  echo '#...'
}

# End a test (arguments directory, name)
end_compare()
{
	if [ "$PRIME" = 1 ]
	then
		return 0
	fi
	mkdir -p test/err/diff
	if { test -r test/out/$NAME.err &&
	     diff -ib test/out/$NAME.out test/nout/$NAME.out >test/err/diff/$NAME &&
	     sed "s|[^ ]*$(/bin/pwd)||" test/nout/$NAME.err |
	     diff -ib test/out/$NAME.err - >>test/err/diff/$NAME ; } ||
	   { test -r test/out/$NAME &&
	     diff -ib test/out/$NAME test/nout/$NAME >test/err/diff/$NAME ; }
	then
		end_test $2 1
	else
		end_test $2 0
		show_error test/err/diff/$NAME
	fi
}

# End a test (arguments result, name)
end_test()
{
	NTEST=`expr $NTEST + 1`
	if [ "$2" = 1 ]
	then
		OK=`expr $OK + 1`
		echo "ok $NTEST - $TEST_GROUP $TEST_NAME"
	else
		NOK=`expr $NOK + 1`
		echo "not ok $NTEST - $TEST_GROUP $TEST_NAME"
		if [ x"$CONTINUE" != x"1" ]
		then
			exit 1
		else
			FAILED="$FAILED $1"
		fi
	fi
}

# Test the analysis of a C project
# runtest name directory csfile
runtest_c()
{
	NAME=$1
	DIR=$2
	SRCPATH=$3
	CSFILE=$4
	start_test $DIR $NAME
	mkdir -p test/err/chunk
(
echo '\p Loading database'
(cd $DIR ; $SRCPATH/$CSCOUT -s hsqldb $CSFILE) 2>test/err/chunk/$NAME.cs
cat <<\EOF
\p Fixing EIDs

SET DATABASE REFERENTIAL INTEGRITY FALSE;

CREATE TABLE FixedIds(EID BIGINT primary key, fixedid integer);

/*
 * Map points Eids as into numbers as follows:
 * 0:	File0 Offset0
 * 1:	File1 Offset0
 * 2:	File2 Offset0
 * ...
 * 134: File134 Offset0
 * 135: File0 Offset1
 * 136: File1 Offset1
 * ...
 *
 * Then map them into odd numbers to ensure they do not clash with existing
 * EIDs, which are even.
 */
INSERT INTO FixedIds
SELECT Eid, Min(Fid + foffset * (select max(Fid) from files)) * 2 + 1
FROM Tokens GROUP BY Eid;

/* select  min(eid), max(eid), fixedid from fixedids group by fixedid having count(fixedid) > 1 */

UPDATE Ids SET Eid=(SELECT FixedId FROM FixedIds WHERE FixedIds.Eid = Ids.Eid);

UPDATE Tokens SET Eid=(SELECT FixedId FROM FixedIds WHERE FixedIds.Eid = Tokens.Eid);

UPDATE IdProj SET Eid=(SELECT FixedId FROM FixedIds WHERE FixedIds.Eid = IdProj.Eid);

UPDATE FunctionId SET Eid=(SELECT FixedId FROM FixedIds WHERE FixedIds.Eid = FunctionId.Eid);

DROP TABLE FixedIds;
\p Fixing FUNCTION IDs
CREATE TABLE FixedIds(FunId BIGINT primary key, FixedId integer);

INSERT INTO FixedIds
SELECT ID, (Fid + foffset * (select max(Fid) from files)) * 2 + 1
FROM Functions;

/* select  min(FunId), max(FunId), fixedid from fixedids group by fixedid having count(fixedid) > 1; */

UPDATE Functions SET id=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Functions.id);

UPDATE FunctionId SET FunctionId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = FunctionId.FunctionId);

UPDATE FunctionMetrics SET FunctionId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = FunctionMetrics.FunctionId);

UPDATE Fcalls SET
SourceId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Fcalls.sourceid),
DestId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Fcalls.DestId);

DROP TABLE FixedIds;

SET DATABASE REFERENTIAL INTEGRITY TRUE;

\p Running selections
\p Table: Ids
SELECT * from Ids WHERE Name != 'PRJ2' ORDER BY Eid;
\p Table: Tokens
SELECT * from Tokens WHERE Fid != 1 ORDER BY Fid, Foffset;
\p Table: Rest
SELECT * from Rest WHERE Fid != 1 ORDER BY Fid, Foffset;
\p Table: Projects
SELECT * from Projects ORDER BY Pid;
\p Table: IdProj
-- All identifiers but PRJ2, which is at variable offsets and hence EIDs
SELECT DISTINCT IdProj.* FROM (
  IdProj LEFT OUTER JOIN (SELECT Eid from Ids WHERE Name = 'PRJ2') X
  on IdProj.Eid = X.Eid) WHERE X.Eid IS NULL
ORDER BY IdProj.Pid, IdProj.Eid;
\p Table: Files
SELECT FID, REGEXP_SUBSTRING(NAME, '[^/]*$') as NAME,
RO, NCHAR, NCCOMMENT, NSPACE, NLCOMMENT, NBCOMMENT, NLINE, MAXLINELEN, NSTRING, NULINE, NPPDIRECTIVE, NPPCOND, NPPFMACRO, NPPOMACRO, NPPTOKEN, NCTOKEN, NCOPIES, NSTATEMENT, NPFUNCTION, NFFUNCTION, NPVAR, NFVAR, NAGGREGATE, NAMEMBER, NENUM, NEMEMBER, NINCFILE
from Files WHERE Fid != 1 ORDER BY Fid;
\p Table: FileProj
SELECT * from FileProj ORDER BY Pid, Fid;
\p Table: Definers
SELECT * from Definers ORDER BY PID, CUID, BASEFILEID, DEFINERID;
\p Table: Includers
SELECT * from Includers ORDER BY PID, CUID, BASEFILEID, IncluderID;
\p Table: Providers
SELECT * from Providers ORDER BY PID, CUID, Providerid;
\p Table: IncTriggers
SELECT * from IncTriggers WHERE Definerid != 1
ORDER BY PID, CUID, Basefileid, Definerid, FOffset;
\p Table: Functions
SELECT * from Functions ORDER BY ID;
\p Table: FunctionMetrics
SELECT * from FunctionMetrics ORDER BY FUNCTIONID;
\p Table: FunctionId
SELECT * from FunctionId ORDER BY FUNCTIONID, ORDINAL;
\p Table: Fcalls
SELECT * from Fcalls ORDER BY SourceID, DESTID;
\p Done
EOF
) |
$HSQLDB mem - |
sed -e '1,/^Running selections/d' >test/nout/$NAME
	end_compare $DIR $NAME
}

# Test the correct dumping of a file's contents into the SQL tables
# runtest name directory csfile
runtest_chunk()
{
	NAME=$1
	DIR=$2
	CSFILE=$3
	start_test $DIR "chunk $NAME"
	mkdir -p test/chunk
(
echo '\p Loading database'
mkdir -p test/err/chunk
(cd $DIR ; $CSCOUT -s hsqldb $CSFILE) 2>test/err/chunk/$NAME.cs
echo '
\p Starting dump
select s from
(select name as s, foffset  from ids inner join tokens on
ids.eid = tokens.eid where fid = 4
union select code as s, foffset from rest where fid = 4
union select comment as s, foffset from comments where fid = 4
union select string as s, foffset from strings where fid = 4
)
order by foffset;
'
) |
$HSQLDB mem - |
sed -e '1,/^Starting dump/d;/^[0-9][0-9]* rows/d' |
tr -d "\n\r" |
perl -pe 's/\\u0000d\\u0000a/\n/g;s/\\u0000a/\n/g' >test/chunk/$NAME
if diff -b test/c/$NAME test/chunk/$NAME >test/err/chunk/$NAME.diff
then
	end_test $NAME 1
else
	end_test $NAME 0
	show_error test/err/chunk/$NAME.diff
	echo " (Make sure the input file uses the correct line-end convention.)"
fi
}

# Create a CScout analysis project file for the given source code file
makecs_c()
{
	if ! [ -d "$IPATH" ] ; then
		echo "Bail out! Unable to access $IPATH directory."
		echo "Working directory is: $(pwd)."
		exit 1
	fi

	echo "
workspace TestWS {
	ipath \"$IPATH\"
	directory test/c {
	project Prj1 {
		file $*
	}
	project Prj2 {
		define PRJ2
		file $*
		file prj2.c
	}
	}
}
" |
perl cswc.pl -d $DOTCSCOUT >makecs.cs 2>/dev/null
}


# Test the preprocessing of a C project
# runtest name csfile directory
runtest_cpp()
{
	NAME=$1
	DIR=$2
	CSFILE=$3
	TOPDIR=$4
	start_test $DIR $NAME
	(cd $DIR ; $TOPDIR/$CSCOUT -3 -E . $CSFILE ) >test/nout/$NAME.out 2>test/nout/$NAME.err
	end_compare $DIR $NAME
}

# Create a CScout preprocessing project file for the given source code file
makecs_cpp()
{
	if ! [ -d "$IPATH" ] ; then
		echo "Bail out! Unable to access $IPATH directory."
		echo "Working directory is: $(pwd)."
		exit 1
	fi

	echo "
workspace TestWS {
	ipath \"$IPATH\"
	ipath \"$CPPTESTS\"
	directory . {
	project Prj1 {
		file $*
	}
}
" |
perl cswc.pl -d $DOTCSCOUT >makecs.cs 2>/dev/null
}

# Set the test control variables to the passed value
set_test()
{
	TEST_AWK=$1
	TEST_RECONST=$1
	TEST_CPP=$1
	TEST_C=$1
	TEST_OBFUSCATION=$1
}

#
# Main script starts here
#

NTEST=0
OK=0
NOK=0

# Parse command-line arguments
set_test 1
while test $# -gt 0; do
        case $1 in
	-p)	PRIME=1
		echo "Priming test data"
		;;
	-k)	CONTINUE=1
		;;
	# One of the values defined in set_test
	-T*)	set_test 0
		var=`echo $1 | sed 's/^-//'`
		eval $var=1
		;;
	esac
	shift
done

CSCOUT=$(dirname $0)/build/cscout

# Adjust HSQLDB_DIR for native Windows Java
if cygpath -a / >/dev/null 2>&1 &&
  ! java -jar $HSQLDB_DIR/lib/sqltool.jar </dev/null >/dev/null 2>&1; then
    HSQLDB_DIR=$(cygpath -w $HSQLDB_DIR)
fi

HSQLDB="java -jar $HSQLDB_DIR/lib/sqltool.jar --rcfile $HSQLDB_DIR/sample/sqltool.rc"
IPATH=$CSCOUT_DIR/include/stdc
CPPTESTS=$CSCOUT_DIR/src/test/cpp
DOTCSCOUT=$CSCOUT_DIR/example/.cscout

chmod 444 ../include/stdc/* ../example/.cscout/*
mkdir -p test/nout

if [ $TEST_RECONST = 1 ]
then
	TEST_GROUP=reconstitution
	# Test reconstitution of individual C files (no priming required)
	for i in ${RCFILES:=$(cd test/c; echo *.c)}
	do
		makecs_c $i
		runtest_chunk $i . makecs.cs
	done
fi

if [ $TEST_CPP = 1 ]
then
	TEST_GROUP=preprocessor
	# Test cases for C preprocessor files
	for i in ${CPPFILES:=$(cd test/cpp; echo *.c)}
	do
		makecs_cpp test/cpp/$i
		runtest_cpp $i . makecs.cs .
	done
	runtest_cpp include_next test/inext driver.cs ../..
fi

# Test cases for individual C files
if [ $TEST_C = 1 ]
then
	TEST_GROUP=C
	for i in ${CFILES:=$(cd test/c; echo *.c)}
	do
		makecs_c $i
		runtest_c $i . . makecs.cs
	done
fi

# Obfuscation
if [ $TEST_OBFUSCATION = 1 ]
then
	TEST_GROUP=obfuscation
	mkdir -p test/err/obfuscation
	(
	cd ../example.obf
	sh run.sh
	cd awk
	MAKEFLAGS= make
	) >test/err/obfuscation/out 2>test/err/obfuscation/err
	if [ -r ../example.obf/awk/awk ]
	then
		end_test awk 1
	else
		end_test awk 0
		show_error test/err/obfuscation/err
	fi
fi

# awk
if [ $TEST_AWK = 1 ]
then
	TEST_GROUP=C
	runtest_c awk.c ../example ../src awk.cs
fi

# Finish priming
if [ "$PRIME" = "1" ]
then
	cp test/nout/* test/out
	# Remove absolute path from error files
	cd test/nout
	for i in *.err ; do
	       sed "s|[^ ]*$(cd ../.. ; pwd)||i" $i >../../test/out/$i
	done

fi

echo "1..$NTEST"
echo "# $OK/$NTEST tests passed"
if [ ! -z "$FAILED" ]
then
	echo "# $NOK/$NTEST tests failed"
	echo "# The following test(s) failed: $FAILED"
fi
