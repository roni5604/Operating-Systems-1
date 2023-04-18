CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: stdshell

stdshell: stdshell.c
	$(CC) $(CFLAGS) -o stshell stshell.c

clean:
	rm -f stshell
