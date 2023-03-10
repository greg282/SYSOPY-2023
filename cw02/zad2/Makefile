CC=gcc
CFLAGS=-Wall -O3


all:
	make output
	make run_with_cmd_args
output:
	$(CC) $(CFLAGS) zad2.c -o zad_2
run_with_cmd_args:
	./zad_2 test_text.txt output.txt >pomiar_zad_2.txt
diff_test:
	./zad_2 test_text.txt tmp1.txt
	./zad_2 tmp1.txt tmp2.txt 
	diff -s tmp2.txt test_text.txt
	rm tmp1.txt
	rm tmp2.txt
tac_test:
	./zad_2 test_text.txt tmp2.txt
	./zad_2 test_text.txt tmp1.txt
	diff -s tmp2.txt tmp1.txt
	rm tmp1.txt
	rm tmp2.txt

clean:
	rm output.txt
	rm pomiar_zad_2.txt