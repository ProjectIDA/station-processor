MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

COPT = 
LIBS =
CFLAGS = -Wall -I.. -I.


LIBS += -lm -lpthread

SRCS = dlresize.c

OBJS = $(SRCS:%.c=%.o)

dlresize : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o dlresize

dlresize.o: ../include/diskloop.h

clean : 
		rm -f *.o dlresize

