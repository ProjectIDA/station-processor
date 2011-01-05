MACHINE = $(shell uname -m)
  CC = gcc -g

COPT =
LIBS =
CFLAGS = -g -Wall -I.. -I. -Iseedlib


LIBS += -lm -lpthread

SRCS = aslreq.c daemonize.c

OBJS = $(SRCS:%.c=%.o)

aslreq : ../lib/dalib.a ../lib/dcctime2lib.a seedlib/seedlib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a seedlib/seedlib.a $(LIBS) -o aslreq

aslreq.o: ../include/diskloop.h ../include/netreq.h

seedlib/seedlib.a:
	cd seedlib; $(MAKE)

clean:
		rm -f *.o aslreq
		cd seedlib; $(MAKE) clean

