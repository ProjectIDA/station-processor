# $Id: Makefile,v 1.5 2005/07/01 00:34:53 dechavez Exp $
# Makefile for SAC utilities

VPATH  =
INCDIR = $(VPATH)../../include
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
LIBS   = $(LIBDIR)/libcssio.a $(LIBDIR)/libsacio.a $(LIBDIR)/libutil.a
OUTPUT = cutsac  makesac \
         nio2sac sac2css sac2nio \
         saca2b  sacb2a  sacecho \
         saclh   sactimes

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

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

cutsac: $(VPATH)cutsac.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)cutsac.c -lsacio -lutil -llogio

makesac: $(VPATH)makesac.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)makesac.c -lsacio -lm

nio2sac: $(VPATH)nio2sac.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)nio2sac.c -lsacio

sac2css: $(VPATH)sac2css.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)sac2css.c -lsacio -lcssio -lutil -llogio

sac2nio: $(VPATH)sac2nio.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)sac2nio.c -lsacio

saca2b: $(VPATH)saca2b.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)saca2b.c -lsacio

sacb2a: $(VPATH)sacb2a.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)sacb2a.c -lsacio

sacecho: $(VPATH)sacecho.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)sacecho.c -lsacio -lutil -llogio

saclh: $(VPATH)saclh.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)saclh.c -lsacio

sactimes: $(VPATH)sactimes.c $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)sactimes.c -lsacio -lutil -llogio
