# $Id: Makefile,v 1.17 2008/01/21 23:08:55 dechavez Exp $
# Makefile for NRTS utilities

VPATH  =
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
LIBSRC = $(VPATH)../../../lib
LIBS   = $(LIBDIR)/libutil.a $(LIBDIR)/libida.a $(LIBDIR)/libnrts.a
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
OUTPUT = nrts_mkdl
OUTPUT += nrts_clear
OUTPUT += nrts_devices
OUTPUT += nrts_dwd
OUTPUT += nrts_fixsys

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

nrts_mkdl: $(VPATH)mkdl.c $(INCDIR)/nrts.h # $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)mkdl.c -lnrts -lida -lisi -lliss -lida10 -lisidb -ldbio -lida -lutil -llogio $(MTLIBS) $(SOCKLIBS) $(SQLLIBS)

nrts_fixsys: $(VPATH)fixsys.c $(INCDIR)/nrts.h
	$(CC) $(CFLAGS) -o $@ $(VPATH)fixsys.c -lnrts -lida -ldbio -lutil -llogio $(MTLIBS) $(SOCKLIBS)

nrts_clear: $(VPATH)clear.c $(INCDIR)/nrts.h # $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)clear.c -lnrts -lida -ldbio -lutil -llogio $(MTLIBS) $(SOCKLIBS)

nrts_dwd: $(VPATH)dwd.c $(INCDIR)/nrts.h # $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)dwd.c -lnrts -lida -lisi -lliss -lida10 -lisidb -ldbio -lcssio -lseed -lutil -llogio $(MTLIBS) $(SOCKLIBS) $(SQLLIBS) -lm

nrts_devices: $(VPATH)devices.c $(INCDIR)/nrts.h # $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)devices.c -lnrts -lida -ldbio -lcssio -lutil -llogio $(MTLIBS) $(SOCKLIBS)

tst: $(VPATH)tst.c $(INCDIR)/nrts.h # $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(VPATH)tst.c -lmsgq -liacp -llogio -lisi -lcssio -lida -lida10 -ldbio -lnrts -lutil -llogio -lnrts -lm
