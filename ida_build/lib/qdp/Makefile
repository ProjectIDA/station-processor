# $Id: Makefile,v 1.14 2007/12/20 22:48:30 dechavez Exp $
# Makefile for qdp library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libqdp.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = $(OUTPUT)(version.o)
OBJS   += $(OUTPUT)(action.o)
OBJS   += $(OUTPUT)(clock.o)
OBJS   += $(OUTPUT)(cmds.o)
OBJS   += $(OUTPUT)(connect.o)
OBJS   += $(OUTPUT)(crc.o)
OBJS   += $(OUTPUT)(data.o)
OBJS   += $(OUTPUT)(debug.o)
OBJS   += $(OUTPUT)(decode.o)
OBJS   += $(OUTPUT)(disk.o)
OBJS   += $(OUTPUT)(encode.o)
OBJS   += $(OUTPUT)(fsa.o)
OBJS   += $(OUTPUT)(hlp.o)
OBJS   += $(OUTPUT)(ida10.o)
OBJS   += $(OUTPUT)(init.o)
OBJS   += $(OUTPUT)(io.o)
OBJS   += $(OUTPUT)(md5.o)
OBJS   += $(OUTPUT)(mem.o)
OBJS   += $(OUTPUT)(meta.o)
OBJS   += $(OUTPUT)(msgq.o)
OBJS   += $(OUTPUT)(par.o)
OBJS   += $(OUTPUT)(pkts.o)
OBJS   += $(OUTPUT)(print.o)
OBJS   += $(OUTPUT)(process.o)
OBJS   += $(OUTPUT)(reorder.o)
OBJS   += $(OUTPUT)(stats.o)
OBJS   += $(OUTPUT)(status.o)
OBJS   += $(OUTPUT)(steim.o)
OBJS   += $(OUTPUT)(string.o)
OBJS   += $(OUTPUT)(tokens.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp
$(OBJS): $(INCDIR)/qdp.h $(INCDIR)/qdp/limits.h $(INCDIR)/qdp/codes.h $(INCDIR)/qdp/fsa.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h $(INCDIR)/qdp/tokens.h action.h

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
 
tests: reboot io decode

FORCE:

reboot: tst.c $(INCDIR)/qdp.h $(INCDIR)/qdp/limits.h $(INCDIR)/qdp/codes.h $(INCDIR)/qdp/fsa.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h $(INCDIR)/qdp/tokens.h FORCE
	$(CC) $(CFLAGS) $(DEFS) -o $@ tst.c -DREBOOT_TEST -L$(LIBDIR) -lqdp -lmsgq -ludpio -lutil -llogio -lmd5 $(MTLIBS) $(SOCKLIBS)
 
decode: tst.c $(INCDIR)/qdp.h $(INCDIR)/qdp/limits.h $(INCDIR)/qdp/codes.h $(INCDIR)/qdp/fsa.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h $(INCDIR)/qdp/tokens.h FORCE
	$(CC) $(CFLAGS) $(DEFS) -o $@ tst.c -DDECODE_TEST -L$(LIBDIR) -lqdp -lmsgq -ludpio -lutil -llogio -lmd5 $(MTLIBS) $(SOCKLIBS)
 
io: tst.c $(INCDIR)/qdp.h $(INCDIR)/qdp/limits.h $(INCDIR)/qdp/codes.h $(INCDIR)/qdp/fsa.h $(INCDIR)/qdp/lcq.h $(INCDIR)/qdp/status.h $(INCDIR)/qdp/tokens.h FORCE
	$(CC) $(CFLAGS) $(DEFS) -o $@ tst.c -DIO_TEST -L$(LIBDIR) -lqdp -lmsgq -ludpio -lutil -llogio -lmd5 $(MTLIBS) $(SOCKLIBS)

FORCE:
