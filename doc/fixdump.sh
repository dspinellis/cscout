#!/bin/sh
#
# Convert selected CScout HTML code to documentation-friendly format
#
winclip -p |
perl -n -e 's/h2>/h3>/gi;
s/h1>/h2>/gi;
s/logo\.png/smlogo.png/g;
s/href="[^"]+"/href="simul.html"/g;
s/ACTION="[^"]+"/ACTION="simul.html"/;
s/CScout \d+\.\d+.*/CScout<\/font>/;
$part .= $_ if (/h2>/../font>/);
$all .= $_;
END {print length($part) ? $part : $all;}' | winclip -c
