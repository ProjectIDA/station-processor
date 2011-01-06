# $Id: Makefile,v 1.5 2004/01/29 18:20:46 dechavez Exp $
# Makefile for seed library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
OPTMIZ  = -g
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libseed.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = \
          $(OUTPUT)(b1000.o) \
          $(OUTPUT)(b1001.o) \
          $(OUTPUT)(decode.o) \
          $(OUTPUT)(fsdh.o) \
          $(OUTPUT)(mini.o) \
          $(OUTPUT)(read.o) \
          $(OUTPUT)(sint.o) \
          $(OUTPUT)(tear.o) \
          $(OUTPUT)(time.o) \
          $(OUTPUT)(toida.o) \
          $(OUTPUT)(type.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/seed.h

makestamp: $(OBJS)
	$(RANLIB) $(OUTPUT)
	touch makestamp
 
clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o
 
remove: OBJS/$(PLATFORM) FORCE
	rm -f $(OUTPUT) OBJS/$(PLATFORM)/makestamp

REMOVE: OBJS FORCE
	rm -rf OBJS $(OUTPUT)

OBJS:
	mkdir $@

$(LIBDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@
 
FORCE:

mini_toida: toida.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ toida.c -lseed -lida -lutil -lm $(SOCKLIBS)

liss_read: read.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ read.c -lseed -lutil -lm $(SOCKLIBS)

test_fsdh: fsdh.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ fsdh.c -lseed -lutil

test_1000: b1000.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ b1000.c -lseed -lutil -lm

test_1001: b1001.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ b1001.c -lseed -lutil -lm
