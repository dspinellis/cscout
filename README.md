[![Build Status](https://travis-ci.org/dspinellis/cscout.svg?branch=master)](https://travis-ci.org/dspinellis/cscout)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8463/badge.svg)](https://scan.coverity.com/projects/dspinellis-cscout)


CScout is a source code analyzer and refactoring browser for collections
of C programs.  It can process workspaces of multiple projects (a project
is defined as a collection of C source files that are linked together)
mapping the complexity introduced by the C preprocessor back into
the original C source code files.  CScout takes advantage of modern
hardware (fast processors and large memory capacities) to analyze
C source code beyond the level of detail and accuracy provided
by  current compilers and linkers.  The analysis CScout performs takes
into account the identifier scopes introduced by the C preprocessor and
the C language proper scopes and namespaces.  CScout has already been
applied on projects of tens of thousands of lines to millions of lines,
like the Linux, OpenSolaris, and FreeBSD kernels, and the Apache web
server.

For more details, examples, and documentation visit the project's
[web site](http://www.spinellis.gr/cscout).

## Building, Testing, Installing, Using
CScout has been compiled and tested on GNU/Linux (Debian jessie),
Apple OS X (El Capitan), FreeBSD (11.0), and Cygwin. In order to
build and use CScout you need a Unix (like) system
with a modern C++ compiler, GNU make, and Perl.
To test CScout you also need to be able to run Java from the command line,
in order to use the HSQLDB database.
To view CScout's diagrams you must have the
[GraphViz](http://www.graphviz.org) dot command in
your executable file path.

* To build run `make`. You can also use the `-j` make option to increase the build's speed.
* To build and test, run `make test`.
* To install (typically after building and testing), run `sudo make install`.
* To see CScout in action run `make example`.

Under FreeBSD use `gmake` rather than `make`.

Testing requires an installed version of _HSQLDB_.
If this is already installed in your system, specify to _make_
the absolute path of the *hsqldb* directory, e.g.
`make HSQLDB_DIR=/usr/local/lib/hsqldb-2.3.3/hsqldb`.
Otherwise, _make_ will automatically download and unpack a local
copy of _HSQLDB_ in the current directory.

## Contributing
* You can contribute to any of the [open issues](https://github.com/dspinellis/cscout/issues) or you can open a new one describing what you want to do.
* For small-scale improvements and fixes simply submit a GitHub pull request.
Each pull request should cover only a single feature or bug fix.
The changed code should follow the code style of the rest of the program.
If you're contributing a feature don't forget to update the documentation.
If you're submitting a bug fix, open a corresponding GitHub issue,
and refer to the issue in your commit.
Avoid gratuitous code changes.
Ensure that the tests continue to pass after your change.
If you're fixing a bug or adding a feature related to the language, add a corresponding test case.
* Before embarking on a large-scale contribution, please open a GitHub issue.
