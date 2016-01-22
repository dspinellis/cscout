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

* To build, run `make`.
* To build and test, run `make test`.
* To install (typically after building and testing), run `sudo make install`.
* To see CScout in action run `make example`.

## Contributing
* For small-scale improvements and fixes simply submit a GitHub pull request.
Each pull request should cover only a single feature or bug fix.
The changed code should follow the code style of the rest of the program.
If you're contributing a feature don't forget to update the documentation.
If you're submitting a bug fix, open a corresponding GitHub issue,
and refer to the issue in your commit.
Avoid gratuitous code changes.
Ensure that the tests continue to pass after your change.
* Before embarking on a large-scale contribution, please open a GitHub issue.
