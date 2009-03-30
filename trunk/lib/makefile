CC=gcc -g -Wl,--hash-style=both
OBJ=\
  disklooplib.o

CFLAGS = -Wall -I..

all: ltime idaapi dalib.a

ltime:
	cd dcc_time2; $(MAKE)

idaapi:
	cd ida; $(MAKE)

disklooplib.o:  disklooplib.c ../include/diskloop.h ../include/netreq.h

dalib.a:  $(OBJ)
	ar -r $@ $(OBJ)

clean:
	/bin/rm -f *.o dalib.a
	cd dcc_time2 ; $(MAKE) clean
	cd ida ; $(MAKE) clean

