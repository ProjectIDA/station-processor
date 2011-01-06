# $Id: Makefile,v 1.7 2004/01/29 18:20:37 dechavez Exp $
# Makefile for sanio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libsanio.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(recv.o)
OBJS   += $(OUTPUT)(send.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(ttag.o)
OBJS   += $(OUTPUT)(util.o)
OBJS   += $(OUTPUT)(version.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

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
 
FORCE:
