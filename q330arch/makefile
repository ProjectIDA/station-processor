MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC=gcc -g
else
  CC=gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../lib330
LIBIDA=../lib/$(PLATFORM)
COPT = 
LIBS = -L$(LIBIDA)
CFLAGS = -Wall -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = q330arch.c daemonize.c server.c logclient.c seedmsg.c
SRCS+= ../dispstatus/statusutil.c

OBJS = $(SRCS:%.c=%.o)

q330arch : ../lib/dalib.a ../lib/dcctime2lib.a \
	../lib/libapi.a $(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a \
	$(LIBIDA)/libisidl.a $(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a \
	$(LIBIDA)/libida.a $(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a \
	$(LIBIDA)/libisidb.a $(LIBIDA)/libdbio.a \
	$(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a $(LIBIDA)/libliss.a \
	$(OBJS) 
	$(CC) -o q330arch $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) \
	../lib/libapi.a $(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a \
	$(LIBIDA)/libisidl.a $(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a \
	$(LIBIDA)/libida.a $(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a \
	$(LIBIDA)/libisidb.a $(LIBIDA)/libdbio.a \
	$(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a $(LIBIDA)/libliss.a \
	-lz -lm -lpthread
		

q330arch.o: ../include/diskloop.h ../include/q330arch.h ../include/shmstatus.h

logclient.o: ../include/q330arch.h

../dispstatus/statusutil.o:	../include/shmstatus.h
	cd ../dispstatus; $(MAKE)

msgtest:  msgtest.o logclient.o seedmsg.o ../lib/dalib.a
	$(CC) msgtest.o logclient.o seedmsg.o \
        ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) \
        -o msgtest

shmget: shmget.o
	$(CC) shmget.o $(LIBS) -o shmget

shmwrite: shmwrite.o
	$(CC) shmwrite.o $(LIBS) -o shmwrite

clean : 
		rm -f *.o q330arch

