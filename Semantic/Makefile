.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -g

default: dana

lexer.c: lexer.l
	flex -s -o lexer.c lexer.l

lexer.o: lexer.c parser.h

parser.h parser.c: parser.y
	bison -dv -o parser.c parser.y

dana: MYast.o general.o error.o symbol.o lexer.o parser.o
	$(CC) $(CFLAGS) -o dana $^ -lfl

clean:
	$(RM) lexer.c parser.c parser.h parser.output *.o *~ *.gch

distclean: clean
	$(RM) dana