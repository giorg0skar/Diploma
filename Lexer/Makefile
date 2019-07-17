.PHONY: clean distclean default

CC=gcc
CFLAGS=-Wall -g

default: dana

lexer.c: lexer.l
	flex -s -o lexer.c lexer.l

dana: lexer.o
	$(CC) $(CFLAGS) -o dana $^ -lfl

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) dana