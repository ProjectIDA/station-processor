# $Id: Makefile,v 1.14 2009/02/23 22:06:02 dechavez Exp $
# Makefile for nrts_edes

VPATH  = 
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../../ReleaseNotes

OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)

LIBS   = -L$(LIBDIR)
LIBS  += -lxfer
LIBS  += -lnrts
LIBS  += -lisi
LIBS  += -lida
LIBS  += -lida10
LIBS  += -lida
LIBS  += -lliss
LIBS  += -lisidb
LIBS  += -lisidl
LIBS  += -lida
LIBS  += -ldbio
LIBS  += -lcssio
LIBS  += -lseed
LIBS  += -lutil
LIBS  += -llogio
LIBS  += -lm
LIBS  += $(SOCKLIBS)
LIBS  += $(SQLLIBS)
LIBS  += $(MTLIBS)

LIBRARIES  = $(LIBDIR)/libxfer.a 
LIBRARIES += $(LIBDIR)/libnrts.a 
LIBRARIES += $(LIBDIR)/libida.a 
LIBRARIES += $(LIBDIR)/libcssio.a 
LIBRARIES += $(LIBDIR)/libutil.a

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += check_wavreq.o
OBJS  += cnfgen1.o
OBJS  += cnfnrts.o
OBJS  += dumppar.o
OBJS  += getrec.o
OBJS  += handlers.o
OBJS  += ida_decode.o
OBJS  += ida10_decode.o
OBJS  += init.o
OBJS  += load_config.o
OBJS  += reformat.o
OBJS  += remap_req.o
OBJS  += search.o
OBJS  += set_indices.o
OBJS  += span.o
OBJS  += wavreq.o
OBJS  += transfer.o

OUTPUT = nrts_edes

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
	rm -f $(BINDIR)/$(OUTPUT)
	cp $(OUTPUT) $(BINDIR)/$(OUTPUT)
	chmod 755 $(BINDIR)/$(OUTPUT)

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

$(OBJS): $(INCDIR)/xfer.h $(INCDIR)/nrts.h

$(OUTPUT): $(OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
