# $Id: Makefile,v 1.6 2007/11/01 21:39:43 dechavez Exp $
# Makefile for sanlog

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../ReleaseNotes
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D_POSIX_PTHREAD_SEMANTICS
DEFS  += -DUSE_OLD_MSGQ
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = -L$(LIBDIR) -lsanio -lutil -llogio $(SOCKLIBS) $(MTLIBS)
OBJS   = main.o
OBJS  += exit.o
OBJS  += flush.o
OBJS  += par.o
OBJS  += sanio.o
OBJS  += signals.o
OUTPUT = sanlog

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

notes: $(VPATH)ReleaseNotes
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes $(NOTESDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) \
  $(LIBDIR)/libsanio.a \
  $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/sanio.h
