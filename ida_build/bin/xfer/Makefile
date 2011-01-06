# $Id: Makefile,v 1.10 2007/01/05 00:47:54 dechavez Exp $
# Makefile for utilities that use the edep service

VPATH  =
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
LIBRARIES   = $(LIBDIR)/libcssio.a
LIBRARIES  += $(LIBDIR)/libsacio.a
LIBRARIES  += $(LIBDIR)/libutil.a
LIBRARIES  += $(LIBDIR)/libseed.a
OUTPUT = xfer_tap xfer_prt xfer_tocss xfer_togse xfer_tosac xfer_toseed eztap

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); rm -rf $(OUTPUT)
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

ars: $(VPATH)ars.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)ars.c -lxfer -lnrts -lida10 -lida -lisidb -ldbio -lseed -lutil -llogio $(SOCKLIBS) -lm

xfer_tap: $(VPATH)tap.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)tap.c -lxfer -lnrts -lida -lisidb -ldbio -lida10 -lisidb -lutil -lseed -llogio $(SOCKLIBS) -lm $(SQLLIBS) $(MTLIBS)

xfer_prt: $(VPATH)prt.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)prt.c -lxfer -lnrts -lida -lisidb -ldbio -lida10 -lisidb -lseed -lutil -llogio $(SOCKLIBS) -lm $(SQLLIBS) $(MTLIBS)

xfer_tocss: $(VPATH)tocss.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)tocss.c -lxfer -lcssio -lutil -llogio $(MTLIBS)

xfer_togse: $(VPATH)togse.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)togse.c -lxfer -lutil -llogio $(MTLIBS)

xfer_tosac: $(VPATH)tosac.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)tosac.c -lxfer -lsacio -lutil -llogio $(MTLIBS)

xfer_toseed: $(VPATH)toseed.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)toseed.c -lxfer -lseed -lutil -llogio -lm $(MTLIBS)

eztap: $(VPATH)eztap.c $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(VPATH)eztap.c -lxfer -lnrts -lida -lisidb -ldbio -lida10 -lisidb -lutil -lseed -llogio $(SOCKLIBS) -lm $(SQLLIBS) $(MTLIBS)
