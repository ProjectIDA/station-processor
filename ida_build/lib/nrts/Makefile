# $Id: Makefile,v 1.13 2009/02/03 22:26:06 dechavez Exp $
# Makefile for nrts library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libnrts.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(asldecode.o)
OBJS   += $(OUTPUT)(bwd.o)
OBJS   += $(OUTPUT)(bwdb.o)
OBJS   += $(OUTPUT)(chksys.o)
OBJS   += $(OUTPUT)(convert.o)
OBJS   += $(OUTPUT)(decode.o)
OBJS   += $(OUTPUT)(die.o)
OBJS   += $(OUTPUT)(dl.o)
OBJS   += $(OUTPUT)(files.o)
OBJS   += $(OUTPUT)(fixwd.o)
OBJS   += $(OUTPUT)(getmap.o)
OBJS   += $(OUTPUT)(ida.o)
OBJS   += $(OUTPUT)(ida10.o)
OBJS   += $(OUTPUT)(ida10decode.o)
OBJS   += $(OUTPUT)(idadecode.o)
OBJS   += $(OUTPUT)(indx.o)
OBJS   += $(OUTPUT)(liss.o)
OBJS   += $(OUTPUT)(mmap.o)
OBJS   += $(OUTPUT)(oldbwd.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(prt.o)
OBJS   += $(OUTPUT)(rcnf.o)
OBJS   += $(OUTPUT)(read.o)
OBJS   += $(OUTPUT)(search.o)
OBJS   += $(OUTPUT)(stawd.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(syswd.o)
OBJS   += $(OUTPUT)(syscode.o)
OBJS   += $(OUTPUT)(sysident.o)
OBJS   += $(OUTPUT)(systems.o)
OBJS   += $(OUTPUT)(time.o)
OBJS   += $(OUTPUT)(wrtdl.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp

$(OBJS): $(INCDIR)/nrts.h $(INCDIR)/nrts/types.h $(INCDIR)/nrts/dl.h

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

test_cmp: datreq.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ datreq.c -lmsgq -liacp -lnrts -llist -llogio -lisi -lcssio -lida -lida10 -lutil -llist -lm
