# Access Control

By default *CScout* only allows the local host (127.0.0.1)
to connect to your server for casual browsing.

To allow other hosts to connect *CScout* features an access control list.
The list is specified in a file called `acl` which
should be located in 
`$CSCOUT_HOME`,
`$HOME/.cscout`, or 
`.cscout` in the current directory.
The list contains lines with IP numeric addresses prefixed by an
`A` (allow)
or
`D` (deny)
prefix and a space.
Matching is performed by comparing a substring of a machine's IP address
against the specified access rule.
Thus an entry such as

```
A 128.135.11.
```

can be used to allow access from a whole subnet.
Unfortunatelly allowing access from the IP address
`192.168.1.1` will
also allow access 
`192.168.1.10`, `192.168.1.100`, and so on.

Allow and deny entries cannot be combined in a useful manner
since the rules followed are:

-  If the address matches an allowed entry the machine will be allowed access.
-  If no allowed entries have been specified,
the machine will be allowed access unless it has been denied access.
(i.e. you can not use a deny entry to exclude a machine from an
allowed group)

Thus you will either specify a restricted list of allowed hosts,
or allow access to the world, specifying a list of denied hosts.
