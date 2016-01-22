hello: hello.c

verify:
	grep '^main	.*hello\.c' tags

clean:
	rm -f hello
