CC=gcc
CFLAGS=-Wall -O3

all:
	make output

output:
	$(CC) $(CFLAGS) zad2.c -o zad2

report:
	echo "----------------------" > report.txt
	./zad2 0.0001 10000 >report.txt
	echo "----------------------" >> report.txt
	./zad2 0.00005 20000 >> report.txt
	echo "----------------------" >> report.txt
	./zad2 0.0000000005 2000 >> report.txt
	
clean:
	rm report.txt
	
