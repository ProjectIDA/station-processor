# $Id: Makefile,v 1.10 2006/03/30 22:41:18 dechavez Exp $
# Makefile for paro

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = -L$(LIBDIR) -lparo -lttyio -llogio -lutil
LIBS  += $(MTLIBS)
LIBS  += $(SOCKLIBS)
OBJS   = ReleaseNotes.o
OBJS  += paro.o
OUTPUT = paro

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

$(OUTPUT): $(OBJS) $(LIBDIR)/libparo.a $(LIBDIR)/libutil.a $(LIBDIR)/libttyio.a 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/paro.h
