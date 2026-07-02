# Obfuscation Back-end

*CScout* can convert a workspace into an obfuscated version.
The obfuscated version of the workspace can be distributed instead
of the original C source, and can be compiled on different processor
architectures and operating systems,
hindering however the code's reverse engineering and modification.

Each source code file is obfuscated by

-  Giving meaningless names to all identifiers: macros, arguments, ordinary identifiers, structure tags, structure members, and labels.

-  Removing comments.

-  Removing extraneous whitespace.

Before running *CScout* to obfuscate, make a complete backup copy of
your source code files, and store them in a secure place;
preferably off-line.
Once the source code files are obfuscated and overwritten, there is no
way to get back their original contents.

To obfuscate the workspace, first ensure that *CScout* can correctly
process the complete set of its source code files.
Use the "unprocessed lines" metric of each file to verify that no
parts of a file are left unprocessed;
unprocessed regions will not be obfuscated.
You can easily increase the coverage of *CScout*'s processing by
including in the workspace multiple projects with different defined
directives.

Also ensure that all your project's files are considered writable, and no
files outside your project (for example system headers) are considered writable.
This will allow *CScout* to rename your identifier names, but
keep the names of library-defined identifiers (for example `printf`)
unchanged.

Finally, run *CScout* with the switch `-o`.
For each writable workspace file *CScout* will create a file ending
in `.obf` that will contain the obfuscated version of its
contents.
The files are not overwritten, providing you with another countermeasure
against accidentally destroying them.
To overwrite the original source with the obfuscated one,
use the following Unix command:

```bash
find . -name '*.obf' |
sed 's/\\/\//g;s/\(.*\)\.obf$/mv "\1.obf" "\1"/' |
sh
```

You can then compile the obfuscated version of your project,
to verify the obfuscation's results.
