#CFLAGS=-w
CFLAGS=-ggdb
LDLIBS=-lm
.PHONY: clean test


all: clean test atg

clean:
	rm -f *.o
	rm -f atg

atg: atg.o IntermediateData.o Error.o BaseFile.o Tree.o DynTab.o Util.o Generator.o Statistics.o

test: test/Makefile
	cd test && make
	

*.o: *.c
