# $Id: Makefile,v 1.12 2006/02/08 23:20:53 dechavez Exp $
# Makefile for ida library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libida.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(cfs.o)
OBJS   += $(OUTPUT)(chnlocmap.o)
OBJS   += $(OUTPUT)(cnfrec.o)
OBJS   += $(OUTPUT)(cnfrec5.o)
OBJS   += $(OUTPUT)(crec.o)
OBJS   += $(OUTPUT)(crec5.o)
OBJS   += $(OUTPUT)(dcmp.o)
OBJS   += $(OUTPUT)(dhead.o)
OBJS   += $(OUTPUT)(dhead1.o)
OBJS   += $(OUTPUT)(dhead2.o)
OBJS   += $(OUTPUT)(dhead3.o)
OBJS   += $(OUTPUT)(dhead4.o)
OBJS   += $(OUTPUT)(dhead5.o)
OBJS   += $(OUTPUT)(dhead6.o)
OBJS   += $(OUTPUT)(dhead7.o)
OBJS   += $(OUTPUT)(dhead8.o)
OBJS   += $(OUTPUT)(dhead9.o)
OBJS   += $(OUTPUT)(dhlen.o)
OBJS   += $(OUTPUT)(drec.o)
OBJS   += $(OUTPUT)(drec1.o)
OBJS   += $(OUTPUT)(drec2.o)
OBJS   += $(OUTPUT)(drec3.o)
OBJS   += $(OUTPUT)(drec4.o)
OBJS   += $(OUTPUT)(drec5.o)
OBJS   += $(OUTPUT)(drec6.o)
OBJS   += $(OUTPUT)(drec7.o)
OBJS   += $(OUTPUT)(drec8.o)
OBJS   += $(OUTPUT)(drec9.o)
OBJS   += $(OUTPUT)(evtrec.o)
OBJS   += $(OUTPUT)(evtrec5.o)
OBJS   += $(OUTPUT)(ext1hzoff.o)
OBJS   += $(OUTPUT)(filltime.o)
OBJS   += $(OUTPUT)(format.o)
OBJS   += $(OUTPUT)(handle.o)
OBJS   += $(OUTPUT)(idrec.o)
OBJS   += $(OUTPUT)(idrec5.o)
OBJS   += $(OUTPUT)(logrec.o)
OBJS   += $(OUTPUT)(logrec5.o)
OBJS   += $(OUTPUT)(rtype.o)
OBJS   += $(OUTPUT)(sampok.o)
OBJS   += $(OUTPUT)(srec.o)
OBJS   += $(OUTPUT)(srec8.o)
OBJS   += $(OUTPUT)(timerr.o)
OBJS   += $(OUTPUT)(timstr.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/ida.h $(INCDIR)/isi.h $(VPATH)/protos.h

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
