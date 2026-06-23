# SQL Back-end

*CScout* can dump the relationships of
an entire workspace in the form of a SQL script.
This can then be uploaded into a relational database for further
querying and processing.

To generate the SQL script simply
run *CScout* with the switch `-s` dialect,
where the argument specifies the SQL dialect (for example,
mysql, or postgresql).
The SQL script will appear in *CScout*'s standard output,
allowing you to directly pipe the results into the database's
client.
For example, say the database you would want to create for your project
was called `myproj`.  

For SQLite you would write:

```bash
rm -f myproj.db
cscout -s sqlite myproj.cs | sqlite3 myproj.db
```

For MySQL you would write:

```bash
(
	echo "create database myproj; use myproj ;"
	cscout -s mysql myproj.cs
) | mysql
```

For PostgreSQL you would write:

```bash
createdb -U username myproj
cscout -s postgres myproj.cs | psql -U username myproj
```

For HSQLDB you would write:

```bash
cscout -s hsqldb myproj.cs |
java -classpath $HSQLDBHOME/lib/hsqldb/hsqldb.jar org.hsqldb.util.SqlTool --rcfile $HSQLDBHOME/lib/hsqldb/sqltool.rc mem -
```

The direct piping allows you to avoid the overhead of creating an
intermediate file, which can be very large.
