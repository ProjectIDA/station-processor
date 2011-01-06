# $Id: Makefile,v 1.16 2006/12/06 22:44:36 dechavez Exp $
# Unix makefile for idalst

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes

OPTMIZ  = -g
INCS   += -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER)
DEFS   += $(SQLDEFS)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR)
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lutil
LIBS   += -llogio

LIBS   += $(SQLLIBS) $(MTLIBS) $(SOCKLIBS) -lm

OBJS    = ReleaseNotes.o
OBJS   += main.o
OBJS   += chktrut.o
OBJS   += ckdupadj.o
OBJS   += ckdupdat.o
OBJS   += ckextjmp.o
OBJS   += cklastw.o
OBJS   += cktinc.o
OBJS   += cktoff.o
OBJS   += cktqual.o
OBJS   += doarceof.o
OBJS   += docalib.o
OBJS   += doconfig.o
OBJS   += dodata.o
OBJS   += dodmp.o
OBJS   += doevent.o
OBJS   += doident.o
OBJS   += dolabel.o
OBJS   += dolog.o
OBJS   += donotdef.o
OBJS   += exitcode.o
OBJS   += help.o
OBJS   += init.o
OBJS   += pbadtag.o
OBJS   += pcalib.o
OBJS   += pconfig.o
OBJS   += pdata.o
OBJS   += pevent.o
OBJS   += pident.o
OBJS   += pinput.o
OBJS   += plog.o
OBJS   += poldcnf.o
OBJS   += pttrip.o
OBJS   += pdunmap.o
OBJS   += pcorrupt.o
OBJS   += dosrec.o
OBJS   += psrec.o
OBJS   += getdat.o
OBJS   += summary.o

OUTPUT = idalst
 
all: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ doinstall
 
clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*
 
remove: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)
 
REMOVE: FORCE
	rm -rf OBJS $(BINDIR)/$(OUTPUT)
 
doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): $(INCDIR)/ida.h $(INCDIR)/util.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
