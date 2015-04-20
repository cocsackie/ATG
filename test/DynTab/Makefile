#CFLAGS=-w
CFLAGS=-ggdb
LDLIBS=-lm
.PHONY: clean runtest


all: clean test runtest

clean:
	rm -f *.o
	rm -f test

test: test.o ../../Error.o ../../DynTab.o
*.o: *.c

runtest: test
	./test
