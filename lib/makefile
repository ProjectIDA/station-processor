MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../../../lib330
OBJ=\
  disklooplib.o\
  map.o

CFLAGS = -Wall -I..

all: ltime dalib.a

ltime:
	cd dcc_time2; $(MAKE)

disklooplib.o:  disklooplib.c ../include/diskloop.h ../include/netreq.h

map.o: map.c ../include/map.h

dalib.a:  $(OBJ)
	ar -r $@ $(OBJ)

clean:
	/bin/rm -f *.o dalib.a
	cd dcc_time2 ; $(MAKE) clean

