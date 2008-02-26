MACHINE = $(shell uname -m)

CC = gcc
COPT = 
LIBS =
CFLAGS = -Wall -I.. -I.


LIBS += -lm -lpthread

SRCS = dlutil.c

OBJS = $(SRCS:%.c=%.o)

dlutil : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o dlutil

dlutil.o: ../include/diskloop.h

clean : 
		rm -f *.o dlutil

