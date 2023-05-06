CC=gcc
CFLAGS=-Wall

INSTALL ?= install
PREFIX ?= /usr
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

SRC=src/wave_encode.c src/1609_3/wave_llc.c src/pdu_buf.c src/fmt_error.c src/1609_3/wsmp_encode.c src/1609_3/wsmp.c \
	test/src/test_main.c test/src/test_wsmp.c test/src/wireless.c src/network.c src/1609_3/wsmp_decode.c src/1609_3/wme_mib.c lib/server.c \
	src/1609_3/wme.c lib/client.c

HDR=include/wave_encode.h include/1609_3/wave_llc.h include/pdu_buf.h include/fmt_error.h include/1609_3/wsmp.h \
	include/1609_3/wsmp_encode.h test/include/test_wsmp.h include/network.h include/1609_3/wsmp_decode.h include/1609_3/wme.h lib/libwave_sock.h

OBJS=wave_encode.o wave_llc.o pdu_buf.o fmt_error.o wsmp_encode.o wsmp.o network.o wsmp_decode.o wme_mib.o wme.o test_wsmp.o client.o server.o

# To build libwave_sock shared library
SOCKSRC=lib/client.c
SOCKHDR=lib/libwave_sock.h
SOCKOBJS=lib/bin/obj/libwave_sock.o
SOCKOS=lib/bin/so/libwave_sock.so

# To run demo examples
DEMO_APP = app/wsa_ex1.c
DEMO_LIBS = -lwave_sock
DEMO_EXEC = app/bin/exec

buildo: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC) 

## for packet capturing
builde: $(OBJS)
	$(CC) $(CFLAGS) -o executable $(OBJS) -lpcap

clean:
	rm -rf *.o *.out executable servexec

## for packet capturing
sendcapture:
	sudo ./executable

genexec: $(OBJS)
	$(CC) $(CFLAGS) -o  servexec $(OBJS) -lpcap

# libwave_sock shared library generation
libSock.o: $(SOCKSRC)
	$(CC) -c -fPIC $(SOCKSRC) -o $(SOCKOBJS)

libClean:
	rm -rf lib/bin/obj/*.o lib/bin/so/*.so

libSock.so: $(SOCKOBJS)
	$(CC) -shared -o $(SOCKOS) $(SOCKOBJS)

gendemo.wsa1:
	$(CC) -o $(DEMO_EXEC) $(DEMO_APP) $(DEMO_LIBS)

run.demo.wsa1: 
	@./$(DEMO_EXEC)

install: $(SOCKOS) $(SOCKHDR)
	$(INSTALL) -D $(SOCKHDR) $(INCLUDEDIR)/libwave_sock.h
	$(INSTALL) -D $(SOCKOS) $(LIBDIR)/libwave_sock.so

uninstall:
	rm $(INCLUDEDIR)/libwave_sock.h
	rm $(LIBDIR)/libwave_sock.so
