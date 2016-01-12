struct foo {
        int foo;
};

struct bar {
        int foo;
};

#define getpart(tag, name) (((struct tag *)p)->name)
#define getfoo(var) (var.foo)
#define get(name) (name(0) + ((struct name *)p)->name)
#define conditional(x) do {if (!x(0)) goto x; return x(0);} while(0)

int
foo(void *p)
{
        struct foo f;
        struct bar b;

foo:
        if (p && getpart(foo, foo))
                return getpart(bar, foo);
        else if (getfoo(f))
                return get(foo);
        else if (getfoo(b))
                conditional(foo);
        else
                return 0;
}
