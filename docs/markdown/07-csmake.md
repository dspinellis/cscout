# Automated Generation of the Processing Script

In *CScout* from version 2.2 and onward you can
you can also use the supplied tool *csmake* to
directly generate *CScout* processing scripts by monitoring a project's
make-based build process.
For this to work your project's build must (probably) be based
on a Unix or Unix-like system, and use *make* and *gcc*.
The make process can also invoke *ld*, *ar*, and *mv*.
Recursive *make* invocations among different directories are
also supported.

The way to use *csmake* is fairly simple.
You first arrange for performing a full build, for example by running

```bash
make clean
```

Then, instead of running `make` on the project's top-level
directory you run `csmake`.
When the build process has finished, *csmake* will leave in
the directory where you started it a *CScout* processing script
named `make.cs`.

*csmake* has been used out-of-the-box to run *CScout* on
the Linux kernel version 2.6.11.4 and the Apache httpd version 2.2.3.
It has also been used to process the FreeBSD 7-CURRENT kernel under
its three Tier-1 architecture configurations by cross-compiling each
configuration separately and merging the resulting *CScout*
processing scripts.
This is the shell script that did the job.

```bash
for a in amd64 i386 sparc64
do
        (
                cd sys/$a/conf/
                make LINT
                config LINT
        )
        export MAKEOBJDIRPREFIX=/home/dds/src/fbsd-head/obj/$a
        csmake buildkernel TARGET_ARCH=$a  KERNCONF=LINT
        mv make.cs make.$a.cs
done
cat make.*.cs >all.cs
sed -n 's/#pragma process "\(.*hack.c\)"/\1/p' all.cs | xargs touch
cscout all.cs
```

	Finally, for processing a couple of C files, you can create a
	project file by invoking the *cscc* tool with the
	arguments you would pass to the C compiler.
