# $Id: Makefile,v 1.5 2005/07/26 00:57:32 dechavez Exp $

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
#OUTPUT  = eom erase mtread \
#         mttest mtwrite offl \
#         fsf fsr rew ttc
OUTPUT = mteom mtfsf mtinit mtstatus

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

mtinit: $(VPATH)mtinit.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mtinit.c -lmtio -lutil

mteom: $(VPATH)eom.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)eom.c -lmtio

mtfsf: $(VPATH)fsf.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)fsf.c -lmtio

mtstatus: $(VPATH)status.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)status.c -lmtio -lutil

mterase: $(VPATH)erase.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)erase.c -lmtio

mtread: $(VPATH)mtread.c $(LIBDIR)/libmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mtread.c -lmtio -lutil

mttest: $(VPATH)mttest.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mttest.c -lmtio

mtwrite: $(VPATH)mtwrite.c $(LIBDIR)/libmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)mtwrite.c -lmtio -lutil

mtoffl: $(VPATH)offl.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)offl.c -lmtio

mtfsr: $(VPATH)fsr.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)fsr.c -lmtio

mtrew: $(VPATH)rew.c $(LIBDIR)/libmtio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)rew.c -lmtio

ttc: $(VPATH)ttc.c $(LIBDIR)/libmtio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)ttc.c -lmtio -lutil
