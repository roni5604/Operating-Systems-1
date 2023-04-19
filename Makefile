CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fPIC -D_GNU_SOURCE
LDFLAGS = -shared

all: encode_decode libcodecA.so libcodecB.so

encode_decode: encode_decode.c
# produces the encode_decode executable with the correct runtime library search path.
	$(CC) $(CFLAGS) -o encode_decode encode_decode.c -ldl -Wl,-rpath,'$$ORIGIN'

libcodecA.so: codecA.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o libcodecA.so codecA.c

libcodecB.so: codecB.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o libcodecB.so codecB.c

clean:
	rm -f encode_decode libcodecA.so libcodecB.so