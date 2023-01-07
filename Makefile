CC=gcc
CFLAGS=-Wall  --std=c99
SRC=src/wave_encode.c src/1609_3/wave_llc.c test/main.c
HDR=include/wave_encode.h include/1609_3/wave_llc.h
OBJS=wave_encode.o wave_llc.o main.o

build: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC)

test: test/main.c $(OBJS)
	$(CC) $(CFLAGS) test/main.c $(OBJS) -o test
clean:
	rm *.o && rm *.out

run:
	gcc main.o wave_encode.o wave_llc.o && ./a.out
