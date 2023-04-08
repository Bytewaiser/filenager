CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99
LDFLAGS=-lcurses

main: main.c
	$(CC) $(CFLAGS) -o filenager main.c $(LDFLAGS)

clean:
	rm -f *.o filenager
