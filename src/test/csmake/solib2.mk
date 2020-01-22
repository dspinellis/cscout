global.so: global.c
	cc -fPIC -shared -o $@ $?

verify:
	grep '^square	.*global\.c' tags

clean:
	rm -f global.so
