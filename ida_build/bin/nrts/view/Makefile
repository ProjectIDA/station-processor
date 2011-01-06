# $Id: Makefile,v 1.4 2004/01/29 18:50:30 dechavez Exp $
# Makefile for nrts_view

VPATH     = 
INCDIR    = $(VPATH)../../../include
BINDIR    = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR    = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ    = -g
INCS      = -I$(INCDIR) $(XINCS)
DEFS      = -D$(OSNAME) -D$(OSTYPE)
CFLAGS    = $(OPTMIZ) $(INCS) $(DEFS)
LIBS      = -L$(LIBDIR) -lnrts -lida -lida10 -lutil -lcssio -lsacio
LIBRARIES = $(LIBDIR)/libnrts.a $(LIBDIR)/libida.a $(LIBDIR)/libutil.a \
            $(LIBDIR)/libcssio.a $(LIBDIR)/libsacio.a

OBJS      = main.o init.o tapinit.o main_menu.o syslist.o viewer_menu.o\
            viewer.o  add_cbs.o draw_cb.o tap.o tocss.o tosac.o togse.o\
            getsys.o utils.o help_cb.o usage.o

OUTPUT = nrts_view

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) rm -f $(BINDIR)/$(OUTPUT)

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

$(OBJS): $(INCDIR)/nrts.h $(INCDIR)/util.h view.h options.h resources.h

$(OUTPUT): $(OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(XLIBS) $(LIBS) $(SOCKLIBS)
