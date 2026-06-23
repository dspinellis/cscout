# C Namespaces

To understand identifier queries it is best to refresh our notion of the
C namespaces.
The main way we normally reuse identifier names in C programs is
through scoping: an identifier within a given scope
such as a block or declared as `static` within a file
will not interfere with identifiers outside that scope.
Thus, the following example will print 3 and not 7.

```c
int i = 3;

foo()
{
	int i = 7;
}

main()
{
	foo();
	printf("%d\n", i);
}
```

*CScout* analyzes and stores each identifier's scope performing
substitutions accordingly.

In addition, C also partitions a program's identifiers into four
*namespaces*.
Identifiers in one namespace, are also considered different from
identifiers in another.
The four namespaces are:

1.  Tags for a `struct/union/enum`
1.  Members of `struct/union`
(actually a separate namespace is assigned
to each `struct/union`)
1.  Labels
1.  Ordinary identifiers (termed *objects* in the C standard)

Thus in the following example all `id` identifier instances are
different:

```c
/* structure tag */
struct id {
	int id;		/* structure member */
};

/* Different structure */
struct id2 {
	char id;	/* structure member */
};

/* ordinary identifier */
id()
{
id:	/* label */
}
```

Furthermore, macro names and the names of macro formal arguments also
live in separate namespaces within the preprocessor.

Normally when you want to locate or change an identifier name,
you only consider identifiers in the same scope and namespace.
Sometimes however,
a C preprocessor macro can semantically unite identifiers
living in different namespaces, so that changes in one of them
should be propagated to the others.
The most common case involves macros that access structure members.

```c
struct s1 {
	int id;
} a;

struct s2 {
	char id;
} b;

#define getid(x) ((x)->id)

main()
{
	printf("%d %c", getid(a), getid(b));
}
```

In the above example, a name change in any of the `id`
instances should be propagated to all others for the program to
retain its original meaning.
*CScout* understands such changes and will propagate any changes
you specify accordingly.

Finally, the C preprocessor's token concatenation feature can result
in identifiers that should be treated for substitution purposes in
separate parts.
Consider the following example:

```c
int xleft, xright;
int ytop, ybottom;

#define coord(a, b) (a ## b)

main()
{
	printf("%d %d %d %d\n",
		coord(x, left),
		coord(x, right),
		coord(y, top),
		coord(y, bottom));
}
```

In the above example, replacing `x` in one of the `coord`
macro invocations should replace the `x` part in the
`xleft` and `xright` variables.
Again *CScout* will recognize and correctly handle this code.
