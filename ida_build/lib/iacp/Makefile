# $Id: Makefile,v 1.5 2008/08/20 18:12:48 dechavez Exp $
# Makefile for iacp library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libiacp.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(accept.o)
OBJS   += $(OUTPUT)(auth.o)
OBJS   += $(OUTPUT)(close.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(free.o)
OBJS   += $(OUTPUT)(get.o)
OBJS   += $(OUTPUT)(handshake.o)
OBJS   += $(OUTPUT)(nrts.o)
OBJS   += $(OUTPUT)(open.o)
OBJS   += $(OUTPUT)(recv.o)
OBJS   += $(OUTPUT)(send.o)
OBJS   += $(OUTPUT)(server.o)
OBJS   += $(OUTPUT)(set.o)
OBJS   += $(OUTPUT)(stats.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(util.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/iacp.h

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
