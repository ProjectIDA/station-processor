# $Id: Makefile,v 1.5 2009/01/05 17:37:41 dechavez Exp $
# Makefile for logio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = liblogio.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS   = $(OUTPUT)(version.o)
OBJS  += $(OUTPUT)(init.o)
OBJS  += $(OUTPUT)(dump.o)
OBJS  += $(OUTPUT)(msg.o)
OBJS  += $(OUTPUT)(string.o)
OBJS  += $(OUTPUT)(threshold.o)
OBJS  += $(OUTPUT)(watchdog.o)

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
