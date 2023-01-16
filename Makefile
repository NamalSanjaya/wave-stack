CC=gcc
CFLAGS=-Wall  --std=c99
SRC=src/wave_encode.c src/1609_3/wave_llc.c test/main.c src/pdu_buf.c src/fmt_error.c
HDR=include/wave_encode.h include/1609_3/wave_llc.h include/pdu_buf.h include/fmt_error.h
OBJS=wave_encode.o wave_llc.o main.o pdu_buf.o fmt_error.o

build: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC)

test: test/main.c $(OBJS)
	$(CC) $(CFLAGS) test/main.c $(OBJS) -o test
clean:
	rm -rf *.o *.out

run:
	gcc main.o wave_encode.o wave_llc.o pdu_buf.o fmt_error.o && ./a.out
