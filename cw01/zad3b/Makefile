CC=gcc
CCFLAGS=-Wall
LIBRARY=../zad1/mylibrary_stats
MAIN=../zad2/repl
LOCALLIB=mylibrary_stats

all: 
	make test_static_1	
	make test_static_2
	make test_static_3

	make test_shared_1
	make test_shared_2
	make test_shared_3

	make test_dynamic_1
	make test_dynamic_2
	make test_dynamic_3


	make report
static_1:
	$(CC) $(CCFLAGS) -c -o1 -o $(LOCALLIB).o $(LIBRARY).c
	ar rcs $(LOCALLIB).a $(LOCALLIB).o
	$(CC) $(CCFLAGS) -c $(MAIN).c
	$(CC) $(CCFLAGS) $(MAIN).o $(LOCALLIB).a -o1 -o main_static_lib

static_2:
	$(CC) $(CCFLAGS) -c -o2 -o $(LOCALLIB).o $(LIBRARY).c
	ar rcs $(LOCALLIB).a $(LOCALLIB).o
	$(CC) $(CCFLAGS) -c $(MAIN).c
	$(CC) $(CCFLAGS) $(MAIN).o $(LOCALLIB).a -o2 -o main_static_lib

static_3:
	$(CC) $(CCFLAGS) -c -o3 -o $(LOCALLIB).o $(LIBRARY).c
	ar rcs $(LOCALLIB).a $(LOCALLIB).o
	$(CC) $(CCFLAGS) -c $(MAIN).c
	$(CC) $(CCFLAGS) $(MAIN).o $(LOCALLIB).a -o3 -o main_static_lib



shared_1:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared  -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC) -o1 $(CCFLAGS)  $(MAIN).c -o main_shared_lib -L . $(LOCALLIB).so


shared_2:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared  -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC) $(CCFLAGS)  -o2 $(MAIN).c  -o main_shared_lib -L . $(LOCALLIB).so


shared_3:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC) $(CCFLAGS)  -o3 $(MAIN).c  -o main_shared_lib -L . $(LOCALLIB).so
	
dynamic_1:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC) -o1  ../zad2/repl_dynamic.c $(LIBRARY).c -ldl -o main_dynamic_lib -I . -D DLL

dynamic_2:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC)  -o2 ../zad2/repl_dynamic.c $(LIBRARY).c -ldl -o main_dynamic_lib -I . -D DLL

dynamic_3:
	$(CC) -c $(CCFLAGS) -Werror -fpic $(LIBRARY).c
	$(CC) -shared -o3 -o $(LOCALLIB).so $(LOCALLIB).o
	$(CC)   -o3 ../zad2/repl_dynamic.c $(LIBRARY).c -ldl  -o main_dynamic_lib -I . -D DLL


clean:
	rm -f *.o *.a *.so

run_static:
	make static
	./main_static_lib

run_shared_1:
	make shared_1
	LD_LIBRARY_PATH=. ./main_shared_lib 

run_shared_2:
	make shared_2
	LD_LIBRARY_PATH=. ./main_shared_lib 
run_shared_3:
	make shared_3
	LD_LIBRARY_PATH=. ./main_shared_lib 
	
run_dynamic_1:
	make dynamic_1
	./main_dynamic_lib
run_dynamic_2:
	make dynamic_2
	./main_dynamic_lib
run_dynamic_3:
	make dynamic_3	
	./main_dynamic_lib


test_static_1:
	make static_1
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

	cat tmp.txt | ./main_static_lib  >results_static_1.txt
	rm tmp.txt

test_static_2:
	make static_2
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

	cat tmp.txt | ./main_static_lib  >results_static_2.txt
	rm tmp.txt

test_static_3:
	make static_3
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

	cat tmp.txt | ./main_static_lib  >results_static_3.txt
	rm tmp.txt



test_shared_1:
	make shared_1
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

	cat tmp.txt | make run_shared_1 >results_shared_1.txt
	rm tmp.txt

test_shared_2:
	make shared_2
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

	cat tmp.txt | make run_shared_2 >results_shared_2.txt
	rm tmp.txt

test_shared_3:
	make shared_3
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

	cat tmp.txt | make run_shared_3 >results_shared_3.txt
	rm tmp.txt
test_dynamic_1:
	make dynamic_1
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

	cat tmp.txt | make run_dynamic_1 >results_dynamic_1.txt
	rm tmp.txt
test_dynamic_2:
	make dynamic_2
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

	cat tmp.txt | make run_dynamic_2 >results_dynamic_2.txt
	rm tmp.txt

test_dynamic_3:
	make dynamic_3
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

	cat tmp.txt | make run_dynamic_3 >results_dynamic_3.txt
	rm tmp.txt

report:
	cat results_dynamic_1.txt results_dynamic_2.txt results_dynamic_3.txt results_shared_1.txt results_shared_2.txt results_shared_3.txt results_static_1.txt results_static_2.txt results_static_3.txt >>report.txt