hello: hello.o

verify:
	grep '^main	.*hello\.c' tags

clean:
	rm -f hello hello.o
