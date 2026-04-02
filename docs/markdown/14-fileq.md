# Generic File Queries

A generic file query is a powerful mechanism for locating files
that match the criteria you specify.
All the ready-made file queries that *CScout* provides you are
just URLs specifying saved instances of generic queries.

You specify the query through the following form:

| Sort-by | Metric | Compare | Value |
| --- | --- | --- | --- |
|  | Number of characters | ignore==!=<> |  |
|  | Number of comment characters | ignore==!=<> |  |
|  | Number of space characters | ignore==!=<> |  |
|  | Number of line comments | ignore==!=<> |  |
|  | Number of block comments | ignore==!=<> |  |
|  | Number of lines | ignore==!=<> |  |
|  | Maximum number of characters in a line | ignore==!=<> |  |
|  | Number of character strings | ignore==!=<> |  |
|  | Number of unprocessed lines | ignore==!=<> |  |
|  | Number of C preprocessor directives | ignore==!=<> |  |
|  | Number of processed C preprocessor conditionals (ifdef, if, elif) | ignore==!=<> |  |
|  | Number of defined C preprocessor function-like macros | ignore==!=<> |  |
|  | Number of defined C preprocessor object-like macros | ignore==!=<> |  |
|  | Number of preprocessed tokens | ignore==!=<> |  |
|  | Number of compiled tokens | ignore==!=<> |  |
|  | Number of copies of the file | ignore==!=<> |  |
|  | Number of statements | ignore==!=<> |  |
|  | Number of defined project-scope functions | ignore==!=<> |  |
|  | Number of defined file-scope (static) functions | ignore==!=<> |  |
|  | Number of defined project-scope variables | ignore==!=<> |  |
|  | Number of defined file-scope (static) variables | ignore==!=<> |  |
|  | Number of complete aggregate (struct/union) declarations | ignore==!=<> |  |
|  | Number of declared aggregate (struct/union) members | ignore==!=<> |  |
|  | Number of complete enumeration declarations | ignore==!=<> |  |
|  | Number of declared enumeration elements | ignore==!=<> |  |
|  | Number of directly included files | ignore==!=<> |  |
|  | Entity name |  |  |

Reverse sort order

Match any of the above
       
Match all of the above
  

---

File names should ( not) 
 match RE

---

Query title 
  

Main page
 — Web: Home
Manual
  

---
CScout

| Sort-by | Metric | Compare | Value |
| --- | --- | --- | --- |
|  | Number of characters | ignore==!=<> |  |
|  | Number of comment characters | ignore==!=<> |  |
|  | Number of space characters | ignore==!=<> |  |
|  | Number of line comments | ignore==!=<> |  |
|  | Number of block comments | ignore==!=<> |  |
|  | Number of lines | ignore==!=<> |  |
|  | Maximum number of characters in a line | ignore==!=<> |  |
|  | Number of character strings | ignore==!=<> |  |
|  | Number of unprocessed lines | ignore==!=<> |  |
|  | Number of C preprocessor directives | ignore==!=<> |  |
|  | Number of processed C preprocessor conditionals (ifdef, if, elif) | ignore==!=<> |  |
|  | Number of defined C preprocessor function-like macros | ignore==!=<> |  |
|  | Number of defined C preprocessor object-like macros | ignore==!=<> |  |
|  | Number of preprocessed tokens | ignore==!=<> |  |
|  | Number of compiled tokens | ignore==!=<> |  |
|  | Number of copies of the file | ignore==!=<> |  |
|  | Number of statements | ignore==!=<> |  |
|  | Number of defined project-scope functions | ignore==!=<> |  |
|  | Number of defined file-scope (static) functions | ignore==!=<> |  |
|  | Number of defined project-scope variables | ignore==!=<> |  |
|  | Number of defined file-scope (static) variables | ignore==!=<> |  |
|  | Number of complete aggregate (struct/union) declarations | ignore==!=<> |  |
|  | Number of declared aggregate (struct/union) members | ignore==!=<> |  |
|  | Number of complete enumeration declarations | ignore==!=<> |  |
|  | Number of declared enumeration elements | ignore==!=<> |  |
|  | Number of directly included files | ignore==!=<> |  |
|  | Entity name |  |  |
|  | Entity name |  |  |  |

You start by specifying whether the file should be
writable (i.e. typically part of your application)
and/or
readable (i.e. typically part of the compiler or system).
Next come a series of metrics *CScout* collects for each
file.
For each metric (e.g. the number of comments) you can specify
an operator `==, !=, <` or `>` and a number
to match that metric against.
Thus to locate files without any comments you would specify  

Number of block comments `==` 0.

On the left of each metric you can specify whether that metric
will be used to sort the resulting file list.
In that case, the corresponding number will appear together with
each file listed.
A separate option allows you to specify that files should be sorted
in the reverse order.

You can request to see files matching any of your specifications
(Match any of the above) or to see files matching all your
specifications
(Match all of the above).

Sometimes you may only want to search in a subset of files;
you can then specify a regular expression that filenames should
match (or not match) against: "File names should (not) match RE".

Finally, you can also specify a title for your query.
The title will then appear on the result document annotating the
results, and will also provide you with a sensible name when creating a
bookmark to it.
