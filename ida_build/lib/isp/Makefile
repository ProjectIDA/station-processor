# $Id: Makefile,v 1.8 2007/01/04 23:34:27 dechavez Exp $
# Makefile for isp library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
OPTMIZ  = -g
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libisp.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(globals.o)
OBJS   += $(OUTPUT)(bootmgr.o)
OBJS   += $(OUTPUT)(cnf.o)
OBJS   += $(OUTPUT)(misc.o)
OBJS   += $(OUTPUT)(param.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(setup.o)
OBJS   += $(OUTPUT)(socket.o)
OBJS   += $(OUTPUT)(status.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/isp.h

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
