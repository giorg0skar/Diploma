.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -g

default: ldana

lexer.c: lexer.l
	flex -s -o lexer.c lexer.l

lexer.o: lexer.c parser.h

parser.h parser.c: parser.y
	bison -dv -o parser.c parser.y

ldana: lexer.o parser.o
	$(CC) $(CFLAGS) -o ldana $^ -lfl

clean:
	$(RM) lexer.c parser.c parser.h parser.output *.o *~ *.gch

distclean: clean
	$(RM) ldana