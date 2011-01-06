# $Id: Makefile,v 1.3 2006/03/13 22:14:26 dechavez Exp $
# Makefile for dbio library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS   += $(SQLDEFS)
DEFS   += -DISI_SERVER
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libdbio.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(close.o)
OBJS   += $(OUTPUT)(ff.o)
OBJS   += $(OUTPUT)(glob.o)
OBJS   += $(OUTPUT)(open.o)
OBJS   += $(OUTPUT)(util.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/dbio.h

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
