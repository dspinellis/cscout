a.out: hello.c
	cc hello.c

verify:
	grep '^main	.*hello\.c' tags

clean:
	rm -f a.out a.exe
