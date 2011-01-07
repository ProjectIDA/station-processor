MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../lib330
COPT = 
LIBS =
CFLAGS = -Wall -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread
OFADUMP_LIBS = $(LIBS)

SRCS = falcon.c logclient.c seedmsg.c daemonize.c \
       alarm.c buffer.c csv.c fmash.c format_data.c \
       get.c herror.c http.c llist.c murmur.c \
       options.c simclist.c url.c util.c \
       falcon_structures.c
OFADUMP_SRCS = ofadump.c record.c format_data.c jio.c fmash.c \
               falcon_structures.c buffer.c simclist.c

OBJS = $(SRCS:%.c=%.o)
OFADUMP_OBJS = $(OFADUMP_SRCS:%.c=%.o)

all: falcon ofadump

falcon : ../lib/dalib.a ../lib/dcctime2lib.a $(OBJS)
	$(CC) $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) -o falcon

ofadump: $(OFADUMP_OBJS)
	$(CC) $(OFADUMP_OBJS) $(LIB330_DIR)/lib330.a $(OFADUMP_LIBS) -o ofadump

falcon.o: ../include/diskloop.h

logclient.o: ../include/diskloop.h ../include/q330arch.h

seedmsg.o:  ../include/netreq.h

clean : 
		rm -f *.o falcon ofadump

