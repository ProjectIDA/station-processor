MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../../../lib330
COPT = 
LIBS =
CFLAGS = -Wall -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = falcon.c logclient.c seedmsg.c daemonize.c

OBJS = $(SRCS:%.c=%.o)

falcon : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o falcon

falcon.o: ../include/diskloop.h

logclient.o: ../include/diskloop.h ../include/q330arch.h

seedmsg.o:  ../include/netreq.h

clean : 
		rm -f *.o falcon

