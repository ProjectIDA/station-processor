# $Id: Makefile,v 1.13 2007/01/04 18:01:26 dechavez Exp $
# Makefile for qdp library
# Modified by Frank Shelly to use as library for q330driver
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

MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC=gcc -g 
else
#  CC=gcc -g -Wl,--hash-style=both
  CC=gcc -g
endif

LIB330_DIR=../../lib330
VPATH   = 
RANLIB  = ranlib
LIBDIR  = ..
OPTMIZ  = -g
INCDIR  = ../../ida_build/include
INCS    = -I$(INCDIR) -I../..
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libapi.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(idaapi.o)

.PRECIOUS : $(OUTPUT)

all: $(OUTPUT) 

$(OUTPUT): makestamp
$(OBJS): $(INCDIR)/qdplus.h ../../include/idaapi.h

makestamp: $(OBJS)
	$(RANLIB) $(OUTPUT)
	touch makestamp
 
clean:
	rm -f *.o
	rm -f $(OUTPUT)

