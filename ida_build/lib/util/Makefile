# $Id: Makefile,v 1.20 2007/01/07 17:41:32 dechavez Exp $
# Makefile for util library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include 
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libutil.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)

OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(atolk.o)
OBJS   += $(OUTPUT)(atolk.o)
OBJS   += $(OUTPUT)(BufferedStream.o)
OBJS   += $(OUTPUT)(basename.o)
OBJS   += $(OUTPUT)(bcd.o)
OBJS   += $(OUTPUT)(bground.o)
OBJS   += $(OUTPUT)(binprint.o)
OBJS   += $(OUTPUT)(case.o)
OBJS   += $(OUTPUT)(cat.o)
OBJS   += $(OUTPUT)(cfgpair.o)
OBJS   += $(OUTPUT)(compress.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(dir.o)
OBJS   += $(OUTPUT)(email.o)
OBJS   += $(OUTPUT)(etoh.o)
OBJS   += $(OUTPUT)(getline.o)
OBJS   += $(OUTPUT)(gsecmp.o)
OBJS   += $(OUTPUT)(hexdump.o)
OBJS   += $(OUTPUT)(htoe.o)
OBJS   += $(OUTPUT)(io.o)
#OBJS   += $(OUTPUT)(isfloat.o)
OBJS   += $(OUTPUT)(isint.o)
OBJS   += $(OUTPUT)(ldump.o)
OBJS   += $(OUTPUT)(lenprt.o)
OBJS   += $(OUTPUT)(list.o)
OBJS   += $(OUTPUT)(lock.o)
OBJS   += $(OUTPUT)(log.o)
OBJS   += $(OUTPUT)(misc.o)
OBJS   += $(OUTPUT)(mkfile.o)
OBJS   += $(OUTPUT)(mkpath.o)
OBJS   += $(OUTPUT)(mmap.o)
OBJS   += $(OUTPUT)(myip.o)
OBJS   += $(OUTPUT)(move.o)
OBJS   += $(OUTPUT)(pack.o)
OBJS   += $(OUTPUT)(parse.o)
OBJS   += $(OUTPUT)(peer.o)
OBJS   += $(OUTPUT)(pow.o)
OBJS   += $(OUTPUT)(query.o)
OBJS   += $(OUTPUT)(rwtimeo.o)
OBJS   += $(OUTPUT)(setid.o)
OBJS   += $(OUTPUT)(signal.o)
OBJS   += $(OUTPUT)(sockinfo.o)
OBJS   += $(OUTPUT)(steim.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(swap.o)
OBJS   += $(OUTPUT)(syserr.o)
OBJS   += $(OUTPUT)(timefunc.o)
OBJS   += $(OUTPUT)(timer.o)

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

test_bcd: bcd.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_TEST -o OBJS/$(PLATFORM)/$@ bcd.c

test_steim1: steim.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_STEIM1 -o OBJS/$(PLATFORM)/$@ steim.c -lseed -lutil -lm

test_steim2: steim.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -DDEBUG_STEIM2 -o OBJS/$(PLATFORM)/$@ steim.c -lseed -lutil -lm
