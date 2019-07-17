.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -g

default: print_code

print_code.o: printAST.c MYast.h
	$(CC) -c printAST.c -o print_code.o

print_code: print_code.o general.o error.o symbol.o
	$(CC) $(CFLAGS) -o print_code $^ -lfl

clean:
	$(RM) *.o *~ *.gch

distclean: clean
	$(RM) print_code