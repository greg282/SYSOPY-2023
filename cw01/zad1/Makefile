CC=gcc
CCFLAGS=-Wall
LIBRARY=mylibrary_stats

all:
	make static
	make shared
static:
	$(CC) $(CCFLAGS) -c -o $(LIBRARY).o $(LIBRARY).c
	ar rcs $(LIBRARY).a $(LIBRARY).o
shared:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LIBRARY).so $(LIBRARY).o

clean:
	rm -f *.o *.a *.so