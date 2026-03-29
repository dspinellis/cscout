# Interfacing with Version Management Systems

When the files *CScout* will modify are under revision control
you may want to check them out for editing before doing the identifier
substitutions, and then check them in again.
*CScout* provides hooks for this operation.
Before a file is modified *CScout* will try to
execute the command `cscout_checkout`;
after the file is modified *CScout* will try to execute the
command `cscout_checkin`.
Both commands will receive as their argument the full path name of the
respective file.
If commands with such names are in your path, they will be executed
performing whatever action you require.

As an example, for a system managed with
[Perforce](https://www.perforce.com/)
the following commands could be used:

## cscout_checkout

```
#!/bin/sh
p4 edit $1
```

## cscout_checkin

```
#!/bin/sh
p4 submit -d 'CScout identifier name refactoring' $1
```
