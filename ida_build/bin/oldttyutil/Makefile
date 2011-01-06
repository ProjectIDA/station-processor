# $Id: Makefile,v 1.1 2005/05/26 23:52:35 dechavez Exp $
# Makefile for old ttyio utilities

VPATH  =
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D$(MACHINE)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)

OUTPUT = ttyio ttytst # ttytst2 ttytst3

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT) wtty rtty
	cd $(BINDIR); rm -f $(OUTPUT) wtty rtty

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 ttyio   $(BINDIR)/rtty
	$(INSTALL) -c -m 755 ttytst  $(BINDIR)/ttytst
#	$(INSTALL) -c -m 755 ttytst2 $(BINDIR)/ttytst2
#	$(INSTALL) -c -m 755 ttytst3 $(BINDIR)/ttytst3
	rm -f $(BINDIR)/wtty
	ln -f $(BINDIR)/rtty $(BINDIR)/wtty

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

ttyio: $(VPATH)ttyio.c $(LIBDIR)/liboldttyio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttyio.c -loldttyio -lutil -lposix4

ttytst: $(VPATH)ttytst.c $(LIBDIR)/liboldttyio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttytst.c -loldttyio -lutil $(SOCKLIBS)

ttytst2: $(VPATH)ttytst2.c $(LIBDIR)/liboldttyio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttytst2.c -loldttyio -lutil $(SOCKLIBS)

ttytst3: $(VPATH)ttytst3.c $(LIBDIR)/liboldttyio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttytst3.c -loldttyio -lutil $(SOCKLIBS)
