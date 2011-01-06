# $Id: Makefile,v 1.9 2005/07/01 00:35:25 dechavez Exp $
# Makefile for NRTS push programs

VPATH  =
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS  += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
OUTPUT = nrts_pushd nrts_push

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd $(BINDIR); rm -f $(OUTPUT)
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

notes: $(VPATH)ReleaseNotes
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes $(NOTESDIR)/nrts_push

rebuild:
	$(MAKE) clean remove all

relink:
	rm -f OBJS/$(PLATFORM)/$(OUTPUT); \
	$(MAKE) all

reinstall:
	$(MAKE) remove install

FORCE:

nrts_pushd: server.o $(VPATH)nrts_push.h
	$(CC) $(CFLAGS) -o $@ server.o -lutil -llogio $(MTLIBS) $(SOCKLIBS)

nrts_push: $(VPATH)client.c $(VPATH)nrts_push.h
	$(CC) $(CFLAGS) -o $@ $(VPATH)client.c -lutil -llogio $(SOCKLIBS)
