# $Id: Makefile,v 1.5 2004/01/29 18:50:05 dechavez Exp $
# Makefile for mag tape utilities

VPATH  =
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D$(MACHINE)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)

OUTPUT  = mtread
OUTPUT += mtwrite
OUTPUT += ttc

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd $(BINDIR); rm -f $(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

mtread: $(VPATH)mtread.c $(LIBDIR)/liboldmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mtread.c -loldmtio -lutil

mtwrite: $(VPATH)mtwrite.c $(LIBDIR)/liboldmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mtwrite.c -loldmtio -lutil

ttc: $(VPATH)ttc.c $(LIBDIR)/liboldmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttc.c -loldmtio -lutil
