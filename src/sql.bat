cd ..\example
..\refactor\i386\cscout -s hsqldb awk.cs >\tmp\awk.sql
cd ..\refactor
java -classpath /app/hsqldb/lib/hsqldb.jar org.hsqldb.util.DatabaseManager -script \tmp\awk.sql
