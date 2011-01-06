# $Id: Makefile,v 1.12 2007/09/25 21:03:29 dechavez Exp $
# Makefile for i10dmx

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = -L$(LIBDIR) -lida10 -lqdp -lisi -lisidb -ldbio -lcssio -llogio -lq330 -ldetect -lutil
LIBS  += $(SQLLIBS)
LIBS  += $(SOCKLIBS)
LIBS  += $(MTLIBS)

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += detect.o
OBJS  += exitcode.o
OBJS  += help.o
OBJS  += init.o
OBJS  += msglog.o
OBJS  += names.o
OBJS  += rdrec.o
OBJS  += tsrec_ok.o
OBJS  += ttag.o
OBJS  += unpackii.o
OBJS  += unpacklm.o
OBJS  += wfdisc.o
OBJS  += wrtdat.o
OUTPUT = i10dmx

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

release: FORCE
	vi ReleaseNotes.c; $(MAKE) remove install

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

notes: $(VPATH)ReleaseNotes.c
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes.c $(NOTESDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) $(LIBDIR)/libida10.a $(LIBDIR)/libutil.a $(LIBDIR)/libcssio.a $(LIBDIR)/libdetect.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/ida10.h $(INCDIR)/detect.h i10dmx.h
