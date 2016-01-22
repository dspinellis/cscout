hello: hello.o global.o
	$(CC) -o $@ $?

verify:
	grep '^main	.*hello\.c' tags
	grep '^global	.*global\.c' tags

clean:
	rm -f hello hello.o global.o
