CC=gcc
CFLAGS=-Wall -fpic -c
OBJECTS=tidyurl.o
LDFLAGS=-shared

pg_tidyurl.so: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm *.o *.so
