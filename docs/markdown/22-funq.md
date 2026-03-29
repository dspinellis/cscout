# Generic Function Queries

The generic function query allows you to select functions by
means of the following form.

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
|  | Number of statements or declarations | ignore==!=<> |  |
|  | Number of operators | ignore==!=<> |  |
|  | Number of unique operators | ignore==!=<> |  |
|  | Number of numeric constants | ignore==!=<> |  |
|  | Number of character literals | ignore==!=<> |  |
|  | Number of if statements | ignore==!=<> |  |
|  | Number of else clauses | ignore==!=<> |  |
|  | Number of switch statements | ignore==!=<> |  |
|  | Number of case labels | ignore==!=<> |  |
|  | Number of default labels | ignore==!=<> |  |
|  | Number of break statements | ignore==!=<> |  |
|  | Number of for statements | ignore==!=<> |  |
|  | Number of while statements | ignore==!=<> |  |
|  | Number of do statements | ignore==!=<> |  |
|  | Number of continue statements | ignore==!=<> |  |
|  | Number of goto statements | ignore==!=<> |  |
|  | Number of return statements | ignore==!=<> |  |
|  | Number of project-scope identifiers | ignore==!=<> |  |
|  | Number of file-scope (static) identifiers | ignore==!=<> |  |
|  | Number of macro identifiers | ignore==!=<> |  |
|  | Total number of object and object-like identifiers | ignore==!=<> |  |
|  | Number of unique project-scope identifiers | ignore==!=<> |  |
|  | Number of unique file-scope (static) identifiers | ignore==!=<> |  |
|  | Number of unique macro identifiers | ignore==!=<> |  |
|  | Number of unique object and object-like identifiers | ignore==!=<> |  |
|  | Number of global namespace occupants at function's top | ignore==!=<> |  |
|  | Number of parameters | ignore==!=<> |  |
|  | Maximum level of statement nesting | ignore==!=<> |  |
|  | Number of goto labels | ignore==!=<> |  |
|  | Fan-in (number of calling functions) | ignore==!=<> |  |
|  | Fan-out (number of called functions) | ignore==!=<> |  |
|  | Cyclomatic complexity (control statements) | ignore==!=<> |  |
|  | Extended cyclomatic complexity (includes branching operators) | ignore==!=<> |  |
|  | Maximum cyclomatic complexity (includes branching operators and all switch branches) | ignore==!=<> |  |
|  | Structure complexity (Henry and Kafura) | ignore==!=<> |  |
|  | Halstead volume | ignore==!=<> |  |
|  | Information flow metric (Henry and Selig) | ignore==!=<> |  |
|  | Entity name |  |  |

Reverse sort order

Match any marked
       
Match all marked
       
Exclude marked
       

Exact match
  

---

| Function names should ( not)   match RE |  |
| --- | --- |
| Names of calling functions should ( not)   match RE |  |
| Names of called functions should ( not)   match RE |  |
| Select functions from filenames ( not)   matching RE |  |

---

Query title 
  

[Main page](simul.md)
 — Web: [Home](simul.md)

[Manual](simul.md)
  

---
CScout

| Function names should ( not)   match RE |  |
| --- | --- |
| Names of calling functions should ( not)   match RE |  |
| Names of called functions should ( not)   match RE |  |
| Select functions from filenames ( not)   matching RE |  |
| Select functions from filenames ( not)   matching RE |  |  |

On the top you can specify whether each function you want listed:

- is a C function

- is a function-like macro

- has a writable declaration

- has a read-only declaration

- is visible in the whole project scope

- is visible only in a file scope

- has a definition body.

As is the case in file queries,
next comes a series of metrics *CScout* collects for each
defined function.
For each metric (e.g. the number of comments) you can specify
an operator `==, !=, <` or `>` and a number
to match that metric against.
Thus to locate functions containing `goto` statement
you would specify  

Number of goto statements `!=` 0.

On the left of each metric you can specify whether that metric
will be used to sort the resulting file list.
In that case, the corresponding number will appear together with
each file listed.
A separate option allows you to specify that files should be sorted
in the reverse order.

Similarly to the identifier query,
you can also specify whether the specified properties should be treated
 

 -  as a disjunction (match any marked),
 -  as a conjunction (match all marked),
 -  as a negation excluding all identifiers matching any property (exclude marked), or
 -  as an exact match specification matching only identifiers that match
 exactly the properties specified (exact match)
 

 

In addition you can specify:

-  That the function should have a specified number of direct callers.

-  A regular expression against which function names should match
(or not match)
-  A regular expression against which the names of calling functions should match
(or not match)
-  A regular expression against which the names of any called functions should match
(or not match)
-  A regular expression that filenames in which functions are declared occur should
match (or not match)
-  A query title to be used for naming the result page.
The title will appear on the result document annotating the
results, and will also provide you with a sensible name when creating a
bookmark to it.
