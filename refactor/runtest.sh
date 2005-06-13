#!/bin/sh
#
# $Id: runtest.sh,v 1.2 2005/06/13 22:46:12 dds Exp $
#


# runtest name csfile directory
runtest()
{
	NAME=$1
	DIR=$2
	CSFILE=$3
	echo "
------------------------------------------
Test $NAME
------------------------------------------
"
(
echo '\p Loading database'
(cd $DIR ; /dds/src/research/cscout/refactor/i386/cscout -s hsqldb $CSFILE)
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
;

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
\p Done
'
) |
java -classpath /app/hsqldb/lib/hsqldb.jar org.hsqldb.util.SqlTool --rcfile C:/APP/hsqldb/src/org/hsqldb/sample/sqltool.rc mem - |
sed -e '1,/^Running selections/d' >test/nout/$NAME
if diff test/out/$NAME test/nout/$NAME
then
	echo "Test $NAME passed" 1>&2
else
	echo "Test $NAME failed" 1>&2
	exit 1
fi
}

# Create a CScout project file for the given source code file
makecs()
{
	echo "
workspace TestWS {
	ipath \"/dds/src/research/CScout/include\"
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
perl prjcomp.pl -d /dds/src/research/CScout/example/.cscout >makecs.cs
}

# Simple files
FILES=`cd test/c; echo *.c`
for i in $FILES
do
	makecs $i
	runtest $i . makecs.cs
done

runtest awk.c ../example awk.cs
