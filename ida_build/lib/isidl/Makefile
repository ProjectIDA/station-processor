# $Id: Makefile,v 1.5 2006/12/12 22:48:42 dechavez Exp $
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
ARCHIVE = libisidl.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(cnf.o)
OBJS   += $(OUTPUT)(datreq.o)
OBJS   += $(OUTPUT)(dl.o)
OBJS   += $(OUTPUT)(glob.o)
OBJS   += $(OUTPUT)(master.o)
OBJS   += $(OUTPUT)(meta.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(search.o)
OBJS   += $(OUTPUT)(soh.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(sys.o)
OBJS   += $(OUTPUT)(tee.o)
OBJS   += $(OUTPUT)(utils.o)
OBJS   += $(OUTPUT)(wfdisc.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/isi.h $(INCDIR)/isi/dl.h $(INCDIR)/nrts/types.h

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
