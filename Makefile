CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: stshell

stshell: stshell.c
	$(CC) $(CFLAGS) -o stshell stshell.c

clean:
	rm -f stshell
