CC=gcc
CFLAGS=-Wall

INSTALL ?= install
PREFIX ?= /usr
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

SRC=src/wave_encode.c src/1609_3/wave_llc.c src/pdu_buf.c src/fmt_error.c src/1609_3/wsmp_encode.c src/1609_3/wsmp.c \
	test/src/test_main.c test/src/test_wsmp.c src/network.c src/1609_3/wsmp_decode.c src/1609_3/wme_mib.c src/controller.c \
	src/1609_3/wme.c lib/client.c src/main.c test/src/test_wme.c

HDR=include/wave_encode.h include/1609_3/wave_llc.h include/pdu_buf.h include/fmt_error.h include/1609_3/wsmp.h \
	include/1609_3/wsmp_encode.h test/include/test_wsmp.h include/network.h include/1609_3/wsmp_decode.h include/1609_3/wme.h lib/libwave_sock.h \
	include/controller.h

OBJS=wave_encode.o wave_llc.o pdu_buf.o fmt_error.o wsmp_encode.o wsmp.o network.o wsmp_decode.o wme_mib.o wme.o test_wsmp.o client.o controller.o main.o

# To build libwave_sock shared library
SOCKSRC=lib/client.c
SOCKHDR=lib/libwave_sock.h
SOCKOBJS=lib/bin/obj/libwave_sock.o
SOCKOS=lib/bin/so/libwave_sock.so

# To run demo examples
DEMO_APP = app/wsa_ex1.c
DEMO_WSMAPP = app/wsm_ex1.c
GPS_TX_APP = app/collision_avoid_tx.c
FINAL_TX_APP = app/demo_tx.c

DEMO_LIBS = -lwave_sock
GPS_TX_LIBS = -lwave_sock -lcurl -lcjson
FINAL_TX_LIBS = -lwave_sock -lm

DEMO_WSAEXEC = app/bin/wsaexec
DEMO_WSMEXEC = app/bin/wsmexec

GPS_TX_DEMO_EXEC = app/bin/gpsexec

FINAL_TX_DEMO_EXEC = app/bin/finaltxexec

buildo: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC) 

## for packet capturing
builde: $(OBJS)
	$(CC) $(CFLAGS) -o executable $(OBJS) -lpcap

clean:
	rm -rf *.o *.out executable servexec test/bin/sckfile test/bin/wave_sckfile

rmsck:
	rm -rf test/bin/sckfile test/bin/wave_sckfile

run.stack:
	sudo ./executable

genexec: $(OBJS)
	$(CC) $(CFLAGS) -o executable $(OBJS) -lpcap -lpthread

# libwave_sock shared library generation
libSock.o: $(SOCKSRC)
	$(CC) -c -fPIC $(SOCKSRC) -o $(SOCKOBJS)

libClean:
	rm -rf lib/bin/obj/*.o lib/bin/so/*.so

libSock.so: $(SOCKOBJS)
	$(CC) -shared -o $(SOCKOS) $(SOCKOBJS)

install: $(SOCKOS) $(SOCKHDR)
	$(INSTALL) -D $(SOCKHDR) $(INCLUDEDIR)/libwave_sock.h
	$(INSTALL) -D $(SOCKOS) $(LIBDIR)/libwave_sock.so

uninstall:
	rm $(INCLUDEDIR)/libwave_sock.h
	rm $(LIBDIR)/libwave_sock.so

# Run app/wsa_ex1.c demo
gen.demo.wsa1:
	$(CC) -o $(DEMO_WSAEXEC) $(DEMO_APP) $(DEMO_LIBS)

run.demo.wsa1: 
	@./$(DEMO_WSAEXEC)

gen.demo.wsm1:
	$(CC) -o $(DEMO_WSMEXEC) $(DEMO_WSMAPP) $(DEMO_LIBS)

run.demo.wsm1: 
	@./$(DEMO_WSMEXEC)

# Final gps application demo tx part
gen.gpstx:
	$(CC) -o $(GPS_TX_DEMO_EXEC) $(GPS_TX_APP) $(GPS_TX_LIBS)

run.gpstx:
	@./$(GPS_TX_DEMO_EXEC)

gen.demotx:
	$(CC) -o $(FINAL_TX_DEMO_EXEC) $(FINAL_TX_APP) $(FINAL_TX_LIBS)

run.demotx:
	@./$(FINAL_TX_DEMO_EXEC)


# Related to stack main function
main.o:
	gcc -c src/controller.c -o src/ctrl.o
	gcc -c src/main.c -o src/main.o

main.e:
	gcc -o main.out src/ctrl.o src/main.o lib/bin/obj/libwave_sock.o -lpthread

mainClean:
	rm -rf src/*.out src/*.o main.out
	rm -rf test/bin/sckfile

run.main:
	@./main.out
