hello-square: global.so hello-square.o
	cc -o $@ hello-square.o global.so

global.so: global.o
	gcc --pic --shared -o $@ $?

verify:
	grep '^main	.*hello-square\.c' tags
	grep '^square	.*global\.c' tags

clean:
	rm -f hello
