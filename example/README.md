This directory allows you to run CScout on the "one true awk" source
code (6600 lines).  The code is almost vanilla ANSI C allowing CScout
to work with the supplied generic ANSI C headers.  Thus, your machine
does not need a C compiler installation, nor do you need to configure
the CScout setup files.  Having entered this directory on the unpacked
distribution tree go to CScout's top level directory and type:

```
make
cd example
../src/build/cscout awk.cs
```

and you are off.

Note: If you have already moved the include file directory somewhere
else, and you have set `CSCOUT_HOME` accordingly, remove the `.cscout`
directory appearing here.

If you feel more adventurous you can processing awk with your system's
native headers. To do this:

* install CScout
* enter the `awk` directory
* run `csmake` to create the CScout processing file,
* run `cscout make.cs`.

I downloaded the awk code in 2003 from Brian Kernighan's home page
<http://cm.bell-labs.com/who/bwk/index.html>
