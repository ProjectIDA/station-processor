MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC=gcc -g
else
  CC=gcc -g
endif

OSUNAME=$(shell uname -s)
ifeq ($(OSUNAME),Linux)
  osname=linux
  OSNAME=LINUX
  OSTYPE=SVR4
  install=/usr/bin/install
  socklibs=
  curselib=-lcurses
  xincs=-I/usr/include/X11
  xlibs=-lXm -lXt -lXext -lX11
  order=LTL_ENDIAN_ORDER
  POSIX4LIB=
  MTLIBS=-lpthread
  machine=i86pc
  platform=linux.2.6.i86pc
else
  osname=untested
endif


LIB330_DIR=../lib330
LIBPCOMM=../libpcomm
LIBIDA=../lib/$(PLATFORM)
COPT = 
LIBS = -L$(LIBIDA)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS = -I.. -I. -I ../ida_build/include $(DEFS)
LIBS += -lm -lpthread

SRCS = archd.c daemonize.c server.c logclient.c seedmsg.c idaapi.c
SRCS+= ../dispstatus/statusutil.c
SRCS+= ../lib/map.c

OBJS = $(SRCS:%.c=%.o)

archd : ../lib/dalib.a ../lib/dcctime2lib.a \
	$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a \
	$(LIBIDA)/libisidl.a $(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a \
	$(LIBIDA)/libida.a $(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a \
	$(LIBIDA)/libisidb.a $(LIBIDA)/libdbio.a \
	$(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a $(LIBIDA)/libliss.a \
    $(LIBPCOMM)/libpcomm.a \
	$(OBJS) 
	$(CC) -o archd $(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a $(LIBS) \
	$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a \
	$(LIBIDA)/libisidl.a $(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a \
	$(LIBIDA)/libida.a $(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a \
	$(LIBIDA)/libisidb.a $(LIBIDA)/libdbio.a \
	$(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a $(LIBIDA)/libliss.a \
    $(LIBPCOMM)/libpcomm.a \
	-lz -lm -lpthread
		

archd.o: ../include/diskloop.h ../include/archd.h ../include/shmstatus.h ../include/map.h

logclient.o: ../include/archd.h

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
		rm -f *.o archd

