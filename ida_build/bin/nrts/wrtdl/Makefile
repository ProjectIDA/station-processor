# $Id: Makefile,v 1.18 2008/01/21 23:08:54 dechavez Exp $
# Makefile for wrtdl

VPATH  = 
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += -D_POSIX_PTHREAD_SEMANTICS
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)

LIBS   = -L$(LIBDIR)
LIBS  += -lxfer
LIBS  += -lnrts
LIBS  += -lisi
LIBS  += -lida
LIBS  += -lida10
LIBS  += -lliss
LIBS  += -lisidb
LIBS  += -lisidl
LIBS  += -ldbio
LIBS  += -lmtio
LIBS  += -liacp
LIBS  += -lcssio
LIBS  += -loldttyio
LIBS  += -lseed
LIBS  += -lutil
LIBS  += -llogio
LIBS  += -lm
LIBS  += $(MTLIBS)
LIBS  += $(POSIX4LIB)
LIBS  += $(SOCKLIBS)
LIBS  += $(SQLLIBS)

LIBRARIES  = $(LIBDIR)/libnrts.a
LIBRARIES += $(LIBDIR)/libisi.a
LIBRARIES += $(LIBDIR)/libcssio.a
LIBRARIES += $(LIBDIR)/libdbio.a
LIBRARIES += $(LIBDIR)/libiacp.a
LIBRARIES += $(LIBDIR)/libseed.a
LIBRARIES += $(LIBDIR)/liblogio.a
LIBRARIES += $(LIBDIR)/libida.a
LIBRARIES += $(LIBDIR)/libida10.a
LIBRARIES += $(LIBDIR)/liboldttyio.a
LIBRARIES += $(LIBDIR)/libmtio.a
LIBRARIES += $(LIBDIR)/libxfer.a

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += exitcode.o
OBJS += flush.o
OBJS += getrec.o
OBJS += init.o
OBJS += isi.o
OBJS += log.o
OBJS += request.o
OBJS += signals.o
OBJS += tee.o
OBJS += ttyio.o

OUTPUT = nrts_wrtdl

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

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

notes: $(VPATH)ReleaseNotes.c
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes.c $(NOTESDIR)/$(OUTPUT)

rebuild:
	$(MAKE) clean remove all

relink:
	rm -f OBJS/$(PLATFORM)/$(OUTPUT); \
	$(MAKE) all

reinstall:
	$(MAKE) remove install

FORCE:

$(OBJS): wrtdl.h $(INCDIR)/nrts.h

$(OUTPUT): $(OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
