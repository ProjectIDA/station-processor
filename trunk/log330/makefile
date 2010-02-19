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

SRCS = log330.c logclient.c seedmsg.c

OBJS = $(SRCS:%.c=%.o)

log330 : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o log330

log330.o: ../include/diskloop.h ../include/q330arch.h

logclient.o: ../include/diskloop.h ../include/q330arch.h

msgtest:  msgtest.o logclient.o seedmsg.o ../lib/dalib.a
	$(CC) msgtest.o logclient.o seedmsg.o \
        ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) \
        -o msgtest

clean : 
		rm -f *.o log330

