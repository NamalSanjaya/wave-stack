CC=gcc
CFLAGS=-Wall
SRC=src/wave_encode.c src/1609_3/wave_llc.c src/pdu_buf.c src/fmt_error.c src/1609_3/wsmp_encode.c src/1609_3/wsmp.c \
	test/src/test_main.c test/src/test_wsmp.c test/src/wireless.c src/network.c src/1609_3/wsmp_decode.c src/1609_3/wme_mib.c test/src/test_wme.c
HDR=include/wave_encode.h include/1609_3/wave_llc.h include/pdu_buf.h include/fmt_error.h include/wave_encode.h include/1609_3/wsmp.h \
	include/1609_3/wsmp_encode.h test/include/test_wsmp.h include/network.h include/1609_3/wsmp_decode.h include/1609_3/ieee1609dot3_mib.h

OBJS=wave_encode.o wave_llc.o pdu_buf.o fmt_error.o wsmp_encode.o wsmp.o test_wme.o network.o wsmp_decode.o wme_mib.o

buildo: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC) 

## for packet capturing
builde: $(OBJS)
	$(CC) $(CFLAGS) -o executable $(OBJS) -lpcap

clean:
	rm -rf *.o *.out executable

## for packet capturing
sendcapture:
	sudo ./executable

testbuild: $(OBJS)
	$(CC) $(CFLAGS) -o executable $(OBJS) -lpcap
