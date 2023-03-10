CC=gcc
CCFLAGS=-Wall
LIBRARY=../zad1/mylibrary_stats
MAIN=repl
LOCALLIB=mylibrary_stats

all: 
	make static
	make shared
	make dynamic
static:
	$(CC) $(CCFLAGS) -c -o $(LOCALLIB).o $(LIBRARY).c
	ar rcs $(LOCALLIB).a $(LOCALLIB).o
	$(CC) $(CCFLAGS) -c $(MAIN).c
	$(CC) $(CCFLAGS) $(MAIN).o $(LOCALLIB).a -o main_static_lib

shared:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC) $(CCFLAGS) $(MAIN).c -o main_shared_lib -L . $(LOCALLIB).so
	
dynamic:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC)  repl_dynamic.c $(LIBRARY).c -ldl -o main_dynamic_lib -I . -D DLL


clean:
	rm -f *.o *.a *.so

run_static:
	make static
	./main_static_lib

run_shared:
	make shared
	LD_LIBRARY_PATH=. ./main_shared_lib 
	
run_dynamic:
	make dynamic
	./main_dynamic_lib