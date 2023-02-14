CC=gcc
CFLAGS=-Wall
SRC=src/wave_encode.c src/1609_3/wave_llc.c src/pdu_buf.c src/fmt_error.c src/1609_3/wsmp_encode.c src/1609_3/wsmp.c \
	test/src/test_main.c test/src/test_wsmp.c
HDR=include/wave_encode.h include/1609_3/wave_llc.h include/pdu_buf.h include/fmt_error.h include/wave_encode.h include/wave.h include/1609_3/wsmp.h include/1609_3/wsmp_encode.h test/include/test_wsmp.h
OBJS=test/bin/wave_encode.o test/bin/wave_llc.o test/bin/pdu_buf.o test/bin/fmt_error.o test/bin/wsmp_encode.o test/bin/wsmp.o test/bin/test_main.o test/bin/test_wsmp.o

build: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC) 

test: test/src/test_main.c $(OBJS)
	$(CC) $(CFLAGS) test/src/test_main.c $(OBJS) -o test
clean:
	rm -rf *.o *.out

run:
	gcc test_main.o wave_encode.o wave_llc.o pdu_buf.o fmt_error.o wsmp_encode.o wsmp.o test_wsmp.o && ./a.out
