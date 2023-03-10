CC=gcc
CFLAGS=-Wall -O3

all:
	make output
	make run_with_cmd_args

output:
	$(CC) $(CFLAGS) zad1.c -o zad_1
run_with_cmd_args:
	./zad_1 J D test_text.txt output.txt > pomiar_zad_1.txt
compare: 
	tr J D < test_text.txt > tmp.txt 
	diff -s tmp.txt output.txt
	rm tmp.txt

clean:
	rm output.txt
	rm pomiar_zad_1.txt