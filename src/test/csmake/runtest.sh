#!/bin/sh
#
# Run the csmake tests
#

echo 'TAP version 13'
echo "1..$(ls *.mk *.mk | wc -l)"

inctest()
{
  n=$(expr $n + 1)
}

mkdir -p test.out
n=0
for i in *.mk ; do
  out="test.out/$(basename $i .mk)"
  rm -f make.cs
  if inctest && csmake -f $i >$out.m.out 2>$out.m.err ; then
    echo "ok $n - csmake $i"
    if inctest && cscout -c make.cs >$out.c.out 2>$out.c.err ; then
      echo "ok $n - cscout $i"
    else
      echo "not ok $n - cscout $i"
      sed 's/^/  /' $out.c.err
    fi
  else
    echo "not ok $n - csmake $i"
    sed 's/^/  /' $out.m.err
  fi
  make -f $i clean >/dev/null
  rm -f make.cs
done
