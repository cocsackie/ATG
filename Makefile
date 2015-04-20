#CFLAGS=-w
CFLAGS=-ggdb
LDLIBS=-lm
.PHONY: clean


all: clean atg

clean:
	rm -f *.o

atg: atg.o IntermediateData.o Error.o BaseFile.o Tree.o DynTab.o Util.o Generator.o Statistics.o

*.o: *.c *.h
