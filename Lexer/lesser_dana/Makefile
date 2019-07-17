.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -g -pg

default: ldana

lexer.c: lexer.l
	flex -s -o lexer.c lexer.l

ldana: lexer.o
	$(CC) $(CFLAGS) -o ldana $^ -lfl

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) ldana