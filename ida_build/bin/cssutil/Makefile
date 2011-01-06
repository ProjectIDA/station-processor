# $Id: Makefile,v 1.8 2007/10/31 17:23:29 dechavez Exp $
# Makefile for CSS utilities

#INSTALL = install
SRCDIR  = ../..
INCDIR  = ../../../../include
BINDIR  = ../../../bin/$(PLATFORM)
LIBDIR  = ../../../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS    = -I$(INCDIR)
CFLAGS  = $(OPTMIZ) $(INCS) -D$(OSNAME) -D$(OSTYPE) -L$(LIBDIR)

OUTPUT  = css2gse
OUTPUT += css2sac
OUTPUT += wdbytes
OUTPUT += wdc
OUTPUT += wdcat
OUTPUT += wdchk
OUTPUT += wdconvert
OUTPUT += wdcut
OUTPUT += wddel
OUTPUT += wdecho
OUTPUT += wdedit
OUTPUT += wdlist
OUTPUT += wdprint
OUTPUT += wdread
OUTPUT += wdspan
OUTPUT += wdtst
OUTPUT += wdjdays

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile BINDIR=../../../../../bin/$(PLATFORM) $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile BINDIR=../../../../../bin/$(PLATFORM) doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)
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

css2gse: $(SRCDIR)/css2gse.c $(LIBDIR)/libcssio.a \
                   $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/css2gse.c -lcssio -lida -lutil -lm

css2sac: $(SRCDIR)/css2sac.c $(LIBDIR)/libcssio.a $(LIBDIR)/libsacio.a \
                   $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/css2sac.c -lcssio -lsacio -lida -lutil  -lm

wdbytes: $(SRCDIR)/wdbytes.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdbytes.c -lcssio -lutil

wdc: $(SRCDIR)/wdc.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdc.c -lcssio -lutil

wdchk: $(SRCDIR)/wdchk.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdchk.c -lcssio -lutil

wdcut: $(SRCDIR)/wdcut.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdcut.c -lcssio -lutil

wddel: $(SRCDIR)/wddel.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wddel.c -lcssio -lutil

wdecho: $(SRCDIR)/wdecho.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdecho.c -lcssio -lutil

wdedit: $(SRCDIR)/wdedit.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdedit.c -lcssio -lutil

wdlist: $(SRCDIR)/wdlist.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdlist.c -lcssio -lutil

wdprint: $(SRCDIR)/wdprint.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdprint.c -lcssio -lutil

wdread: $(SRCDIR)/wdread.c $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdread.c -lcssio -lutil

wdconvert: $(SRCDIR)/wdconvert.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdconvert.c -lcssio -lutil

wdcat: $(SRCDIR)/wdcat.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdcat.c -lcssio -lutil -llogio -lm

wdspan: $(SRCDIR)/wdspan.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdspan.c -lcssio -lutil

wdtst: $(SRCDIR)/wdtst.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdtst.c -lcssio -lutil

wdjdays: $(SRCDIR)/wdjdays.c $(LIBDIR)/libcssio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/wdjdays.c -lcssio -lutil
