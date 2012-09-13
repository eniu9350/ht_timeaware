CC=gcc
BINDIR=../bin
BINNAME=ht

ht: clean
	$(CC) --std=c99 -g -o $(BINDIR)/$(BINNAME) -I/usr/include -I/usr/local/include/gsl -I../include -L. -L/usr/local/lib  hash.c hashtable.c main.c -lm -lgsl -lgslcblas


clean:
	rm $(BINDIR)/$(BINNAME) test e l of -rf