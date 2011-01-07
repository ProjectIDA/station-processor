MACHINE = $(shell uname -m)
# Needed for IDA code
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
OSUNAME=$(shell uname -s)
LIBIDA=../lib/$(platform)

DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT

# End IDA stuff

ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

COPT = 
LIBS = -L $(LIBIDA)
CFLAGS = -I.. -I. -I../ida_build/include $(DEFS)
LIBS += -lm -lpthread -lz

SRCS = isirstat.c

OBJS = $(SRCS:%.c=%.o)

isirstat : $(OBJS) \
		../lib//libapi.a \
    $(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
    $(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
    $(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
    $(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
    $(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o isirstat \
		../lib//libapi.a \
    $(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
    $(LIBIDA)/libnrts.a $(LIBIDA)/libisi.a $(LIBIDA)/libida10.a \
    $(LIBIDA)/libida.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
    $(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
    $(LIBIDA)/libcssio.a $(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a

isirstat.o: ../include/diskloop.h

clean : 
		rm -f *.o isirstat

