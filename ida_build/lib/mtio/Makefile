# $Id: Makefile,v 1.5 2004/01/29 18:20:30 dechavez Exp $
# Makefile for mtio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
OPTMIZ  = -g
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DMACHINE=$(MACHINE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libmtio.a
OUTPUT  = $(LIBDIR)/libmtio.a
OBJS    = $(OUTPUT)(mtio.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

test: test_mtio

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

test_mtio: mtio.c
	cd OBJS/$(PLATFORM); \
	$(CC) -g -o $@ ../../mtio.c -I$(INCD) $(DEFS) -DDEBUG_TEST $(LIBS)
 
FORCE:
