header-a: header-a.c

verify:
	grep '^main	.*header-a\.c' tags

clean:
	rm -f header-a
