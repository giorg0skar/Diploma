.PHONY: clean distclean

CC=gcc
CFLAGS=-Wall
files= loop fact reverse_string sort nothing hanoi pass_by_reference

all: $(files)

loop: loop.c
	$(CC) $(CFLAGS) $< -o $@

fact: fact.c
	$(CC) $(CFLAGS) $< -o $@

reverse_string: reverse_string.c
	$(CC) $(CFLAGS) $< -o $@

sort: sort.c
	$(CC) $(CFLAGS) $< -o $@

nothing: nothing.c
	$(CC) $(CFLAGS) $< -o $@

hanoi: hanoi.c
	$(CC) $(CFLAGS) $< -o $@

pass_by_reference: pass_by_reference.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o *~

distclean: clean
	$(RM) $(files)