MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC=gcc -g
else
  CC=gcc -g -Wl,--hash-style=both
endif

COPT =
LIBS =
CFLAGS = -Wall -I.. -I.


LIBS += -lm -lpthread

SRCS = netreq.c daemonize.c

OBJS = $(SRCS:%.c=%.o)

netreq : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o netreq

netreq.o: ../include/diskloop.h ../include/netreq.h

clean : 
		rm -f *.o netreq

