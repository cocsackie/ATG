#CFLAGS=-w
CFLAGS=-ggdb
.PHONY: clean


all: clean atg

clean:
	rm -f *.o

atg: atg.o IntermediateData.o Error.o BaseFile.o Tree.o DynTab.o Util.o Generator.o

*.o: *.c *.h
