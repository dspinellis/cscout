# Examples of SQL Queries

Once you have loaded the *CScout*-generated SQL data into a relational
database (see [SQL Back-end](33-sql.md)), you can run queries to
analyze your codebase.

The following query lists functions ordered by their number of
statements.

```sql
SELECT name, nstmt
FROM FUNCTIONS
INNER JOIN FUNCTIONMETRICS ON FUNCTIONS.ID = FUNCTIONMETRICS.FUNCTIONID
ORDER BY nstmt DESC;
```

To find files with the most lines of code:

```sql
SELECT name, nline
FROM FILES
INNER JOIN FILEMETRICS ON FILES.FID = FILEMETRICS.FILEID
ORDER BY nline DESC;
```

To list function call relationships:

```sql
SELECT f1.name AS caller, f2.name AS callee
FROM FCALLS
INNER JOIN FUNCTIONS f1 ON FCALLS.SOURCEID = f1.ID
INNER JOIN FUNCTIONS f2 ON FCALLS.DESTID = f2.ID
ORDER BY f1.name, f2.name;
```

For the full database schema, see
[Schema of the Generated Database](34-dbschema.md).
For instructions on generating and loading the SQL data, see
[SQL Back-end](33-sql.md).
