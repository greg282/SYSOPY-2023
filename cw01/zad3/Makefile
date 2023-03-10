CC=gcc
CCFLAGS=-Wall
LIBRARY=../zad1/mylibrary_stats
MAIN=../zad2/repl
LOCALLIB=mylibrary_stats

all: 
	make test_static
	make test_shared
	make test_dynamic
	make report
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
	$(CC)  ../zad2/repl_dynamic.c $(LIBRARY).c -ldl -o main_dynamic_lib -I . -D DLL


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

make test_static:
	make static
	find ../  -type f -name '*.c'  | xargs readlink -f  >tmp.txt	
	sed -i 's/^/count /' tmp.txt
	sed -i '0,/^/s//init 10000 \n/' tmp.txt
	echo "show 0" >> tmp.txt
	echo "show 1" >> tmp.txt
	echo "show 2" >> tmp.txt
	echo "delete 0" >> tmp.txt
	echo "delete 1" >> tmp.txt
	echo "delete 2" >> tmp.txt
	echo "destroy" >> tmp.txt

	cat tmp.txt | ./main_static_lib  >results_static.txt``
	rm tmp.txt

test_shared:
	make shared
	find ../  -type f -name '*.c'  | xargs readlink -f  >tmp.txt	
	sed -i 's/^/count /' tmp.txt
	sed -i '0,/^/s//init 10000 \n/' tmp.txt
	echo "show 0" >> tmp.txt
	echo "show 1" >> tmp.txt
	echo "show 2" >> tmp.txt
	echo "delete 0" >> tmp.txt
	echo "delete 1" >> tmp.txt
	echo "delete 2" >> tmp.txt
	echo "destroy" >> tmp.txt

	cat tmp.txt | make run_shared >results_shared.txt``
	rm tmp.txt

test_dynamic:
	make dynamic
	find ../  -type f -name '*.c'  | xargs readlink -f  >tmp.txt	
	sed -i 's/^/count /' tmp.txt
	sed -i '0,/^/s//init 10000 \n/' tmp.txt
	echo "show 0" >> tmp.txt
	echo "show 1" >> tmp.txt
	echo "show 2" >> tmp.txt
	echo "delete 0" >> tmp.txt
	echo "delete 1" >> tmp.txt
	echo "delete 2" >> tmp.txt
	echo "destroy" >> tmp.txt

	cat tmp.txt | make run_dynamic >results_dynamic.txt``
	rm tmp.txt
report:
	cat results_dynamic.txt results_shared.txt results_static.txt >>report.txt