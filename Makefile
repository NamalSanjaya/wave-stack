CC=gcc
CFLAGS=-Wall  --std=c99
SRC=src/wave_sap.c src/1609_3/llc/llc_sap.c test/main.c src/ds/hashtb.c src/fmt_error.c
HDR=include/wave_sap.h include/1609_3/llc/llc_sap.h include/ds/hashtb.h include/fmt_error.h
OBJS=wave_sap.o llc_sap.o main.o hashtb.o fmt_error.o

build: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC)

test: test/main.c $(OBJS)
	$(CC) $(CFLAGS) test/main.c $(OBJS) -o test
clean:
	rm -rf *.o *.out sckfile

run:
	gcc -pthread main.o wave_sap.o llc_sap.o hashtb.o fmt_error.o && ./a.out
