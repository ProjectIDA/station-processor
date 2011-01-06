# $Id: Makefile,v 1.4 2004/01/29 18:50:34 dechavez Exp $
# Makefile for nrts_xfer

VPATH  = 
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = -L$(LIBDIR) -lnrts -lida -lida10 -lseed -lcssio -lutil -lm
LIBRARIES = $(LIBDIR)/libnrts.a $(LIBDIR)/libida.a $(LIBDIR)/libcssio.a \
            $(LIBDIR)/libutil.a $(LIBDIR)/libseed.a

OBJS   = main.o cnfreq.o datreq.o exitcode.o getindex.o getrec.o init.o \
         search.o span.o wdreq.o

OUTPUT = nrts_xfer

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

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

$(OBJS): nrts_xfer.h $(INCDIR)/nrts.h

$(OUTPUT): $(OBJS) $(LIBRARIES)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(SOCKLIBS)
