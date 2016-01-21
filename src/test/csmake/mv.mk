hello: hello.o moved.o

moved.o: global.c
	cc -c global.c
	mv global.o moved.o

clean:
	rm -f hello hello.o moved.o
