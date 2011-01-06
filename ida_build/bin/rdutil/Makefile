# $Id: Makefile,v 1.7 2005/07/01 00:34:52 dechavez Exp $

VPATH   = 
INSTALL = install
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ  = -g
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
CFLAGS  = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
OUTPUT  = rdread rdwrite rdinit rdeject #rdtest

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

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

rdread: $(VPATH)read.c $(LIBDIR)/librdio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)read.c -lrdio -lutil -llogio

rdwrite: $(VPATH)write.c $(LIBDIR)/librdio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)write.c -lrdio -lutil -llogio

#rdtest: $(VPATH)test.c $(LIBDIR)/librdio.a
#	$(CC) $(CFLAGS) -o $@ $(VPATH)test.c -lrdio -lutil -llogio

rdinit: $(VPATH)init.c $(LIBDIR)/librdio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)init.c -lrdio -lutil -llogio

rdeject: $(VPATH)eject.c $(LIBDIR)/librdio.a
	$(CC) $(CFLAGS) -o $@ $(VPATH)eject.c -lrdio -lutil -llogio
