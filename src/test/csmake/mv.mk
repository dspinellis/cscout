hello: hello.o moved.o

verify:
	grep '^main	.*hello\.c' tags
	grep '^global	.*global\.c' tags

moved.o: global.c
	cc -c global.c
	mv global.o moved.o

clean:
	rm -f hello hello.o moved.o
