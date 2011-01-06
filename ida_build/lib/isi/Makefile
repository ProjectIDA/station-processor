# $Id: Makefile,v 1.11 2009/05/14 16:33:13 dechavez Exp $
# Makefile for isi library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS   += -DISI_SERVER
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libisi.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(cnf.o)
OBJS   += $(OUTPUT)(datreq.o)
OBJS   += $(OUTPUT)(ezio.o)
OBJS   += $(OUTPUT)(fileio.o)
OBJS   += $(OUTPUT)(log.o)
OBJS   += $(OUTPUT)(netutil.o)
OBJS   += $(OUTPUT)(open.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(raw.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(seqno.o)
OBJS   += $(OUTPUT)(soh.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(ts.o)
OBJS   += $(OUTPUT)(utils.o)
OBJS   += $(OUTPUT)(wfdisc.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/isi.h

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
