# $Id: Makefile,v 1.5 2004/01/29 18:20:35 dechavez Exp $
# Makefile for rdio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D$(MACHINE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = librdio.a
OUTPUT  = $(LIBDIR)/librdio.a
OBJS    = $(OUTPUT)(rdio.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

test: test_rdio

$(OUTPUT): makestamp

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

# For testing

LIBD = ../../$(LIBDIR)
INCD = ../../$(INCDIR)
LIBS = -L$(LIBD) -lutil

test_rdio: rdio.c
	cd OBJS/$(PLATFORM); \
	$(CC) -g -o $@ ../../rdio.c -I$(INCD) $(DEFS) -DDEBUG_TEST $(LIBS)
 
FORCE:
