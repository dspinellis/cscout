hello: hello.a
	cc -o $@ $?

hello.a: hello.o
	ar r $@ $?

verify:
	grep '^main	.*hello\.c' tags

clean:
	rm -f hello
