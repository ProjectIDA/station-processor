MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  LIB330_DIR = /home/q330dev/lib330
else
  LIB330_DIR = /usr/local/lib/lib330
endif
LIB330_DIR=$(HOME)/lib330

# Needed for IDA code
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

CC = gcc -g
COPT = 
LIBS = -L$(LIB330_DIR) -L $(LIBIDA) -l330
CFLAGS = -I.. -I. -I$(HOME)/ida_build/include $(DEFS)
LIBS += -lm -lpthread

SRCS = ida2chan.c

OBJS = $(SRCS:%.c=%.o)

ida2chan : $(OBJS) \
		../lib//libapi.a \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a $(LIBIDA)/libida.a \
		$(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o ida2chan -lz -lm -lpthread \
		../lib//libapi.a \
		$(LIBIDA)/libqdp.a $(LIBIDA)/libqdplus.a $(LIBIDA)/libisidl.a \
		$(LIBIDA)/libnrts.a $(LIBIDA)/libida10.a $(LIBIDA)/libida.a \
		$(LIBIDA)/libisi.a $(LIBIDA)/libiacp.a $(LIBIDA)/libisidb.a \
		$(LIBIDA)/libdbio.a $(LIBIDA)/libutil.a $(LIBIDA)/liblogio.a \
		$(LIBIDA)/libliss.a \
		../lib/dalib.a ../lib/dcctime2lib.a

ida2chan.o: ../include/diskloop.h

$(LIB330_DIR)/lib330.a : 
	@echo
	@echo "lib330 has not been built, or can not be found in any"
	@echo "of the system library paths, or in $(LIB330_DIR)."
	@echo
	@exit -1

clean : 
		rm -f *.o ida2chan

