
all: x

x.o: x.c
	gcc -v -c -o $@ $< -Ii1 -Ii2 -Ii3 -Ii4

x: x.o
	gcc -v -o $@ $<

clean:
	rm -f x.o x
