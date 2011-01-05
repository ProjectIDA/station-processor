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

SRCS = netseed.c daemonize.c wildmatch.c

OBJS = $(SRCS:%.c=%.o)

netseed : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o netseed

netseed.o: ../include/diskloop.h ../include/netreq.h

clean : 
		rm -f *.o netseed

