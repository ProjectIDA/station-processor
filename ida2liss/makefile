MACHINE = $(shell uname -m)

# Needed for IDA code
OSUNAME=$(shell uname -s)
LIBIDA=../lib/$(PLATFORM)
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

ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

COPT =
LIBS = -L $(LIBIDA)
CFLAGS = -Wall -I.. -I. -I../ida_build/include $(DEFS)
LIBS += -lm -lpthread

SRCS = ida2liss.c netsend.c wildmatch.c daemonize.c

OBJS = $(SRCS:%.c=%.o)

ida2liss : $(OBJS) \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
		$(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o ida2liss -lz -lm -lpthread \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
		$(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a

ida2liss.o: ../include/diskloop.h

netsend.o:

wildmatch.o:

clean : 
		rm -f *.o ida2liss

