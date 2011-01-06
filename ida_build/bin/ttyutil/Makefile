# $Id: Makefile,v 1.12 2007/01/05 00:47:54 dechavez Exp $
# Unix makefile for rtty/wtty

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR) -lttyio -lutil -llogio
LIBS   += $(MTLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = ttyio
 
all: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ doinstall
 
clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*
 
remove: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)
 
REMOVE: FORCE
	rm -rf OBJS $(BINDIR)/$(OUTPUT)
 
doinstall: $(OUTPUT)
	rm -f $(BINDIR)/rtty $(BINDIR)/wtty
	cp $(OUTPUT) $(BINDIR)/rtty
	ln -f $(BINDIR)/rtty $(BINDIR)/wtty
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): $(INCDIR)/ttyio.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libttyio.a $(LIBDIR)/libutil.a $(LIBDIR)/liblogio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
