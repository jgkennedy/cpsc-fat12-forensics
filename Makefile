CC=clang
CFLAGS=-Wall -g

BINS= notjustcats

all: $(BINS)

notjustcats: notjustcats.c
	$(CC) $(CFLAGS) notjustcats.c -o notjustcats

clean:
	rm *.o *~ *.h.gch $(BINS)
