# $Id: Makefile,v 1.9 2005/10/11 22:41:25 dechavez Exp $
# Makefile for cssio library

VPATH   = 
RANLIB  = ranlib
OPTMIZ  = -g
INCS    = -I$(VPATH)../../include -I$(VPATH)../../include/css
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libcssio.a
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(css2sac.o)
OBJS   += $(OUTPUT)(datatype.o)
OBJS   += $(OUTPUT)(desc.o)
OBJS   += $(OUTPUT)(jdate.o)
OBJS   += $(OUTPUT)(rwfdisc.o)
OBJS   += $(OUTPUT)(rwfdrec.o)
OBJS   += $(OUTPUT)(togse.o)
OBJS   += $(OUTPUT)(wdcut.o)
OBJS   += $(OUTPUT)(wf28to30.o)
OBJS   += $(OUTPUT)(wf30to28.o)
OBJS   += $(OUTPUT)(wwfdisc.o)
OBJS   += $(OUTPUT)(wrdsize.o)

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
