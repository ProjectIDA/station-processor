# $Id: Makefile,v 1.3 2007/01/04 21:38:36 judy Exp $
# Makefile for isi library database routines

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS   += $(SQLDEFS)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libisidb.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(calib.o)
OBJS   += $(OUTPUT)(chan.o)
OBJS   += $(OUTPUT)(chnlocmap.o)
OBJS   += $(OUTPUT)(coords.o)
OBJS   += $(OUTPUT)(datlen.o)
OBJS   += $(OUTPUT)(debug.o)
OBJS   += $(OUTPUT)(inst.o)
OBJS   += $(OUTPUT)(revs.o)
OBJS   += $(OUTPUT)(root.o)
OBJS   += $(OUTPUT)(sint.o)
OBJS   += $(OUTPUT)(site.o)
OBJS   += $(OUTPUT)(sitechan.o)
OBJS   += $(OUTPUT)(sitechanflag.o)
OBJS   += $(OUTPUT)(srate.o)
OBJS   += $(OUTPUT)(stamap.o)
OBJS   += $(OUTPUT)(system.o)
OBJS   += $(OUTPUT)(tqual.o)
OBJS   += $(OUTPUT)(trecs.o)
OBJS   += $(OUTPUT)(wrdsiz.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/isi.h $(INCDIR)/isi/db.h $(INCDIR)/dbio.h

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
