MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  LIB330_DIR = ../../../lib330
else
  LIB330_DIR = ../../../lib330
endif

CC = gcc -g -Wl,--hash-style=both
COPT = 
LIBS =
CFLAGS = -Wall -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = log330.c logclient.c seedmsg.c

OBJS = $(SRCS:%.c=%.o)

log330 : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o log330

log330.o: ../include/diskloop.h ../include/log330.h

logclient.o: ../include/diskloop.h ../include/log330.h

msgtest:  msgtest.o logclient.o seedmsg.o ../lib/dalib.a
	$(CC) msgtest.o logclient.o seedmsg.o \
        ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) \
        -o msgtest

shmget: shmget.o
	$(CC) shmget.o $(LIBS) -o shmget

shmwrite: shmwrite.o
	$(CC) shmwrite.o $(LIBS) -o shmwrite

clean : 
		rm -f *.o log330

