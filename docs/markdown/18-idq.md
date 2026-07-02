# Generic Identifier Queries

The generic identifier query feature of *CScout* is one of
its most powerfull features, allowing you to accurately specify
the properties of identifiers you are looking for, by means of
the following form.

| Identifier names should ( not)   match RE |  |
| --- | --- |
| Select identifiers from filenames ( not)   matching RE |  |

---

Query title 
  

[Main page](simul.html)
 — Web: [Home](simul.html)
[Manual](simul.html)
  

---
CScout

| Identifier names should ( not)   match RE |  |
| --- | --- |
| Select identifiers from filenames ( not)   matching RE |  |
| Select identifiers from filenames ( not)   matching RE |  |  |

In the form you specify:

-  The properties (namespace, scope, instances) of the identifier
-  Whether the specified properties should be 
treated 
 

   -  as a disjunction (match any marked),
   -  as a conjunction (match all marked),
   -  as a negation excluding all identifiers matching any property (exclude marked), or
   -  as an exact match specification matching only identifiers that match
 exactly the properties specified (exact match)
 

-  A regular expression against which identifier names should match
(or not match)
-  A regular expression that filenames in which identifiers occur should
match (or not match)
-  A query title to be used for naming the result page.
The title will appear on the result document annotating the
results, and will also provide you with a sensible name when creating a
bookmark to it.

Through the query's submission button can choose to obtain as a result

-  the identifiers that match the specific query, 

-  the files containing identifiers that match the query, or

-  the functions containing identifiers that match the query.

In the second case (matching files),
each file in the file list will provide you with
a link (marked source) showing the file's source code with all matched
identifiers marked using hyperlinks.

As an example, the following query could be used to identify
unused file-scoped writable identifiers, but excluding
the `copyright` and `rcsid` identifiers:

| Identifier names should ( not)   match RE |  |
| --- | --- |
| Select identifiers from filenames matching RE |  |

---

Query title 
  

[Main page](simul.html)
  

---
CScout 1.16 - 2003/08/17 12:13:01

| Identifier names should ( not)   match RE |  |
| --- | --- |
| Select identifiers from filenames matching RE |  |
| Select identifiers from filenames matching RE |  |  |
