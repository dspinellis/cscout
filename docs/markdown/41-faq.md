# Frequently Asked Questions

## Contents

-  [How do I handle conditional compilation?](#cond)

-  [How can I handle automatically generated files?](#auto)

-  [How can I save an identifier or file query?](#save)

-  [Why aren't my call graphs appearing in the form I specified?](#cgfmt)

-  [How can I locate and fix a syntax error?](#syntax)

-  [Why is my read-only prefix pragma not working under Windows?](#win32fname)

-  [My system appears to be using the hard disk excessively (thrashing).  Why?](#mem)

-  [Why can't I see SVG call graphs in my browser?](#svgview)

-  [Isn't the CScout logo infringing the intellectual property of the International Scout movement?](#logo)

<a id="cond"></a> 
## How do I handle conditional compilation?

You can either define macros that will cover all conditional cases,
or process the same project multiple times using different macro
definitions.
See [this page](short.md).

<a id="auto"></a> 
## How can I handle automatically generated files?

Some projects use mini domain-specific languages
similar to *yacc* and *lex* to express some of their
elements.
*CScout* can natively parse C and *yacc* source files, but
no other language.
Obviously changes should be performed in the original domain-specific
files, rather than the generated C code.
On the other hand, *CScout* can not parse the original files,
but can parse the generated code.
To escape this situation include the automatically generated file
in your workspace definition, but define it as read-only.
In this way *CScout* will not allow you to modify identifiers appearing
in it.

<a id="save"></a> 
## How can I save an identifier or file query?

Simply bookmark the page that shows the query's results.
You can even pass the URL around or print it on a T-shirt;
the URL contains the whole query.

<a id="cgfmt"></a> 
## Why aren't my call graphs appearing in the form I specified?

Changing the global options that specify the format of call graphs
affects the types of links appearing in the corresponding pages.
If you go to a previous page using the back button and you do not
reload it, you will use the old links and will obtain the old type
of call graph.
This is the only instance where the use of the back button will
surprise you.

<a id="syntax"></a> 
## How can I locate and fix a syntax error?

Most syntax errors occur due to compiler extensions or incorrect
definitions of macros.
They can often be corrected, by introducing a dummy macro that gets around
the corresponding compiler extension, like the following.

```
#define __declspec(x)
```

If the problem isn't obvious from the source code, you might need
preprocess the file by using the `-E` option,
and look at the preprocessed code.
To do this search in the preprocessed code for an (ideally unique)
occurrence of non-macro code near the problem spot.

<a id="win32fname"></a> 
## Why is my read-only prefix pragma not working under Windows?

Filename matching under Windows is a difficult subject.
Filenames retain case, but are compared in a case insensitive manner.
To avoid problems, when writing `ro_prefix` and `ipath`
pragmas under a Windows platform, respect the following rules.

-  Include a drive letter in the path.

-  Write the drive letter in uppercase.

-  Separate the path elements using a single backslash.

-  Do not end the path with a trailing backslash.

<a id="mem"></a> 
## My system appears to be using the hard disk excessively (thrashing).  Why?

*CScout* tags and follows each and every identifier of the source
code it processes, including header files.
As a result, the memory requirements of *CScout* are considerable.
Typical memory requirements are 700-1600 bytes per line processed.
If your system's main memory is less than the ammount needed,
*CScout* will page to disk and thrashing will occur.

<a id="svgview"></a> 
## Why can't I see SVG call graphs in my browser?

Make sure your browser supports SVG viewing.
For instance, Firefox and Internet Explorer work fine; SeaMonkey doesn't.

<a id="tabkey"></a> 
## Why doesn't the Tab key in the Safari browser allow me to move to each identifier definition?

Press `Option` (or `Alt`) `Tab`, instead of
Tab.
You can also permanently change Safari's behavior under
*Safari - Preferences - Advanced*.

<a id="logo"></a> 
## Isn't the CScout logo infringing the intellectual property of the International Scout movement?

The emblem of the International Scout movement is based on
the *fleur de lys*,
a traditional design of the 11th century that was later used
to decorate maps and compass cards.
CScout acts as a compass and a map for C code, so the association
with the *fleur de lys* is particularly relevant.
You can read more about the *fleur de lys* symbol at the
[Wikipedia](http://www.wikipedia.org/wiki/Fleur-de-lis)
web site entry.
Places or institutions
that use the symbol informally or as part of their heraldic arms are:
Quebec; Canada; Augsburg, Germany; Florence, Italy; Slovenia; the
Fuggers medieval banking family; Bosnia and Herzegovina; and Louisville,
Kentucky; the Prince of Wales also has a fleur de lys on his coat of
arms.
A
[Google image search](http://images.google.com.gr/images?q=fleur+de+lys&hl=en&lr=&ie=ISO-8859-1&safe=on) for the image
will also show you tens of similar designs.
Finally, note that the International Scout movement's
rendering of the image includes two stars on the left and right leaves.
CScout's logo does not contain this distinctive feature.
