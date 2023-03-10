CC=gcc
CFLAGS=-Wall -O3

all:
	make output
	make run
output:
	$(CC) $(CFLAGS) zad3.c -o zad_3
run:
	./zad_3
compare_test:
	echo "Command wc --bytes * result:">compare_test.txt
	wc --bytes * >>compare_test.txt
	echo "-------------------------">>compare_test.txt
	echo "Function result:" >> compare_test.txt
	./zad_3 >>compare_test.txt
clean:
	rm compare_test.txt