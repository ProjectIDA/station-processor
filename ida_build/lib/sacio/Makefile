# $Id: Makefile,v 1.6 2004/01/29 18:20:36 dechavez Exp $
# Makefile for sacio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libsacio.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = \
         $(OUTPUT)(rah.o)  \
         $(OUTPUT)(rbh.o)  \
         $(OUTPUT)(time.o) \
         $(OUTPUT)(wad.o)  \
         $(OUTPUT)(wah.o)  \
         $(OUTPUT)(wbh.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/sacio.h

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
