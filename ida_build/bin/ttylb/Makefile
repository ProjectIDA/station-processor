# $Id: Makefile,v 1.8 2005/07/01 00:34:55 dechavez Exp $
# Makefile for ttylb

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = $(MTLIBS) -L$(LIBDIR) -lttyio -llogio -lutil -llogio
OBJS   = main.o read.o write.o
OUTPUT = ttylb

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

$(OUTPUT): $(OBJS) $(LIBDIR)/libttyio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): ttylb.h
