CC=gcc
CFLAGS=-Wall -O3

all:
	make output

output:
	$(CC) $(CFLAGS) zad3.c -o zad3
	$(CC) $(CFLAGS) calculate.c -o calculate


report:
	echo "----------------------" > report.txt
	./zad3 0.0001 10000 >report.txt
	echo "----------------------" >> report.txt
	./zad3 0.00005 2000 >> report.txt
	echo "----------------------" >> report.txt
	./zad3 0.00001 10000 >> report.txt
	
clean:
	rm report.txt
	

	