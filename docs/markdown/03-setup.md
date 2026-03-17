# Installation and Setup

## System Requirements

To build run *CScout* your system must satisfy the following
requirements:

	- Have a relatively modern C++ compiler that supports
	the `-std=c++11` flag.  *CScout*
	has been built and tested both with GCC and LLVM.
-  Have an installation of the [Perl](http://www.perl.org)
interpreter
-  Contain the include files of the compiler your programs use

-  Support a Web browser (text or graphics-based).
*CScout* uses vanilla HTML and has been tested with
Firefox, SeaMonkey, Mozilla, Safari,
the Microsoft Internet Explorer, and Lynx.
-  (Optional, needed only for displaying call and dependency graphs.)
Have an installation of the AT&T
[GraphViz](http://www.graphviz.org) *dot*
program in the executable file path, and a Web browser supporting,
directly or via a plugin, the display of SVG (scalable vector graphics)
content.
-  Have SQLite installed.
	(This is required for testing.)

## Installation and Configuration

From this point onward we use the term Unix to refer to Unix-like systems
like GNU/Linux and FreeBSD, and Windows to refer to Microsoft Windows
systems.

You install *CScout* in five steps:

	1.  Clone the source code from [GitHub](https://github.com/dspinellis/cscout/)

	1.  Enter the *CScout* directory with `cd cscout`

	1.  Run `make`

	1.  Run `make test` (optional, but highly recommended)

	1.  Run `sudo make install`. If you want the installation
to use a different directory hierarchy than the default
`/usr/local`
, you can specify this on the command
line with the `PREFIX` variable.
For example, you run `make install PREFIX=/home/mydir`
to install *CScout* under your home directory or
`sudo make install PREFIX=/usr`
to install *CScout* under /usr.

	By default the installation will create in `/usr/local/include/cscout`
	headers corresponding to a generic standard C compilation and to your
	host's specific configuration.
	If you want to process programs based on other host configurations
	you can modify these files or create a local version of the files
	in your home or the project's current directory.

In most cases you want *CScout* to process your code using
the include files of the compiler you are normally using.
This will allow *CScout* to handle programs using the libraries
and facilities available in your environment
(e.g. Unix system calls or the Windows API).
In the other hand it exposes *CScout* to the extensions and quirks
that might reside in your system's header files.
You can typically reslove these problems by adding a few additional
macro definitions that neutralize unknown compiler extensions.
As an example,
if your compiler supports a `quad_double` type and associated
keyword with semantics roughly equivalent to `double`
you would add a line in `host-defs.h`:

```
#define quad_double double
```

Have a look in the existing `host-defs.h` file to see
what might be required.
If your programs are written in standard C and do not use any additional
include files, you can use the generic header files.
