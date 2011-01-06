# $Id: Makefile,v 1.4 2009/02/23 21:48:43 dechavez Exp $
# Makefile for qdplus library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libqdplus.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(decode.o)
OBJS   += $(OUTPUT)(digitizer.o)
OBJS   += $(OUTPUT)(handle.o)
OBJS   += $(OUTPUT)(io.o)
OBJS   += $(OUTPUT)(meta.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(process.o)
OBJS   += $(OUTPUT)(state.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp
$(OBJS): $(INCDIR)/qdplus.h $(INCDIR)/qdp.h

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
