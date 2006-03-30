#!/bin/sh
#
# $Id: runtest.sh,v 1.8 2006/03/30 15:55:09 dds Exp $
#


# Start a test (arguments directory, name)
start_test()
{
	echo "
------------------------------------------
Test $2 begins
"
}

# End a test (arguments directory, name)
end_test()
{
	if [ "$PRIME" = "1" ]
	then
		return 0
	fi
	if diff -b test/out/$NAME test/nout/$NAME
	then
		echo "
Test $2 finishes correctly
------------------------------------------
"
	else
		echo "
Test $2 failed
------------------------------------------
"
		if [ "$CONTINUE" != "1" ]
		then
			exit 1
		fi
	fi
}

# Test the analysis of a C project
# runtest name directory csfile
runtest_c()
{
	NAME=$1
	DIR=$2
	CSFILE=$3
	start_test $DIR $NAME
(
echo '\p Loading database'
(cd $DIR ; $CSCOUT -s hsqldb $CSFILE)
echo '
\p Fixing EIDs
CREATE TABLE FixedIds(EID integer primary key, fixedid integer);

/*
 * Map points Eids as into numbers as follows:
 * 0:	File0 Offset0
 * 1:	File1 Offset0
 * 2:	File2 Offset0
 * ...
 * 1234: File134 Offset0
 * 1235: File0 Offset1
 * 1236: File1 Offset1
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
CREATE TABLE FixedIds(FunId integer primary key, FixedId integer);

INSERT INTO FixedIds
SELECT ID, (Fid + foffset * (select max(Fid) from files)) * 2 + 1
FROM Functions;

/* select  min(FunId), max(FunId), fixedid from fixedids group by fixedid having count(fixedid) > 1; */

UPDATE Functions SET id=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Functions.id);

UPDATE FunctionId SET FunctionId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = FunctionId.FunctionId);

UPDATE Fcalls SET
SourceId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Fcalls.sourceid),
DestId=(SELECT FixedId FROM FixedIds WHERE FixedIds.FunId = Fcalls.DestId);

DROP TABLE FixedIds;
\p Running selections
SELECT * from Ids ORDER BY Eid;
SELECT * from Tokens ORDER BY Fid, Foffset;
SELECT * from Rest ORDER BY Fid, Foffset;
SELECT * from Projects ORDER BY Pid;
SELECT * from IdProj ORDER BY Pid, Eid;
SELECT * from Files ORDER BY Fid;
SELECT * from FileProj ORDER BY Pid, Fid;
SELECT * from Definers ORDER BY PID, CUID, BASEFILEID, DEFINERID;
SELECT * from Includers ORDER BY PID, CUID, BASEFILEID, IncluderID;
SELECT * from Providers ORDER BY PID, CUID, Providerid;
SELECT * from IncTriggers ORDER BY PID, CUID, Basefileid, Definerid, FOffset;
SELECT * from Functions ORDER BY ID;
SELECT * from FunctionId ORDER BY FUNCTIONID, ORDINAL;
SELECT * from Fcalls ORDER BY SourceID, DESTID;
\p Done
'
) |
$HSQLDB mem - |
sed -e '1,/^Running selections/d' >test/nout/$NAME
	end_test $DIR $NAME
}

# Create a CScout analysis project file for the given source code file
makecs_c()
{
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
perl prjcomp.pl -d $DOTCSCOUT >makecs.cs
}


# Test the preprocessing of a C project
# runtest name csfile directory
runtest_cpp()
{
	NAME=$1
	DIR=$2
	CSFILE=$3
	start_test $DIR $NAME
(cd $DIR ; $CSCOUT -E $CSFILE 2>&1 ) >test/nout/$NAME
	end_test $DIR $NAME
}

# Create a CScout preprocessing project file for the given source code file
makecs_cpp()
{
	echo "
workspace TestWS {
	ipath \"$IPATH\"
	directory test/cpp {
	project Prj1 {
		file $*
	}
}
" |
perl prjcomp.pl -E -d $DOTCSCOUT >makecs.cs
}

#
# Main script starts here
#

# Parse command-line arguments
while test $# -gt 0; do
        case $1 in
	-p)	PRIME=1
		;;
	-k)	CONTINUE=1
		;;
	esac
	shift
done

# Set host-dependent variables
case `hostname` in
eagle)
	CSCOUT=/dds/src/research/cscout/refactor/i386/cscout
	HSQLDB="java -classpath /app/hsqldb/lib/hsqldb.jar org.hsqldb.util.SqlTool --rcfile C:/APP/hsqldb/src/org/hsqldb/sample/sqltool.rc"
	IPATH=/dds/src/research/CScout/include
	DOTCSCOUT=/dds/src/research/CScout/example/.cscout
	;;
sense)
	CSCOUT=$HOME/src/cscout/sparc/cscout
	HSQLDB="java -classpath $HOME/lib/hsqldb/hsqldb.jar org.hsqldb.util.SqlTool --rcfile $HOME/lib/hsqldb/sqltool.rc"
	IPATH=$HOME/src/include
	DOTCSCOUT=$HOME/src/example/.cscout
	;;
esac

# See that we are running a version of CScout that supports SQL dumps
:>/tmp/empty
if ! $CSCOUT -s hsqldb $CSFILE /tmp/empty 2>/dev/null
then
	echo 'CScout is not compiled with DEBUG=1 or LICENSEE=...' 1>&2
	exit 1
fi
rm -f /tmp/empty

# Test cases for individual C files
FILES=`cd test/c; echo *.c`
for i in $FILES
do
	makecs_c $i
	runtest_c $i . makecs.cs
done

# awk
runtest_c awk.c ../example awk.cs

# Test cases for C preprocessor files
FILES=`cd test/cpp; echo *.c`
for i in $FILES
do
	makecs_cpp $i
	runtest_cpp $i . makecs.cs
done

# Finish priming
if [ "$PRIME" = "1" ]
then
	cp test/nout/* test/out
fi
