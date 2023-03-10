CC=gcc
CFLAGS=-Wall -O3

all:
	make output

output:
	$(CC) $(CFLAGS) zad4.c -o zad_4
compare_test:
	echo "Command result">compare.txt
	find . -type f|xargs wc --bytes >> compare.txt
	echo "------------------------------">>compare.txt
	echo "Function result">>compare.txt
	./zad_4 . >> compare.txt
clean:
	rm compare.txt