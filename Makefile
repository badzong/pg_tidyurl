CC=gcc
CFLAGS=-Wall -I/home/manuel/Downloads/postgresql-9.0.4/src/include -fpic -c
OBJECTS=sanitize.o
LDFLAGS=-shared

sanitize_url.so: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm *.o *.so
