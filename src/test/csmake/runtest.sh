#!/bin/sh
#
# Run the csmake tests
#

echo 'TAP version 13'
echo "1..$(ls *.mk *.mk *.mk | wc -l)"

inctest()
{
  n=$(expr $n + 1)
}

mkdir -p test.out
n=0
for i in *.mk ; do
  out="test.out/$(basename $i .mk)"
  rm -f make.cs tags
  if inctest && csmake -f $i >$out.m.out 2>$out.m.err ; then
    echo "ok $n - csmake $i"
  else
    echo "not ok $n - csmake $i"
    sed 's/^/  /' $out.m.err
  fi
  if inctest && cscout -Cc make.cs >$out.c.out 2>$out.c.err ; then
    echo "ok $n - cscout $i"
  else
    echo "not ok $n - cscout $i"
    sed 's/^/  /' $out.c.err
  fi
  if inctest && make -f $i verify >$out.v.out 2>$out.v.err ; then
    echo "ok $n - verify $i"
  else
    echo "not ok $n - verify $i"
    sed 's/^/  /' $out.v.err
  fi
  make -f $i clean >/dev/null
  rm -f make.cs tags
done
