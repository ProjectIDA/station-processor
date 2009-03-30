# Needed for IDA code
MACHINE = $(shell uname -m)
OSUNAME=$(shell uname -s)
LIBIDA=$(HOME)/lib/$(PLATFORM)
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
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
# End IDA stuff

CC = gcc -g -Wl,--hash-style=both
COPT = 
LIB330_DIR = ../../../lib330
LIBS = -L $(LIBIDA)
CFLAGS = -I.. -I. -I$(HOME)/ida_build/include $(DEFS) -I$(LIB330_DIR) 
LIBS += -lz -lm -lpthread

SRCS = ida2arch.c daemonize.c wildmatch.c

OBJS = $(SRCS:%.c=%.o)

ida2arch:	$(OBJS) ../lib/dalib.a ../lib/dcctime2lib.a \
		../lib//libapi.a \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
		$(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a
	$(CC) -o $@ $(OBJS) $(LIBS) ../lib/dalib.a ../lib/dcctime2lib.a \
		../lib/libapi.a \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
		$(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a
		
ida2arch.o: ../include/diskloop.h

wildmatch.o:

../dispstatus/statusutil.o:	../include/shmstatus.h
	cd ../dispstatus; $(MAKE)

clean : 
		rm -f *.o ida2arch

