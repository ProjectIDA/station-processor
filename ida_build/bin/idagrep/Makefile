# $Id: Makefile,v 1.1 2009/02/04 17:57:25 dechavez Exp $
# Unix makefile for idagrep

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(EWFLAGS)

LIBS    = -L$(LIBDIR) -lida -lida10 -ldbio -lutil -llogio

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = idagrep
 
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
	rm -f $(BINDIR)/$(OUTPUT)
	cp $(OUTPUT) $(BINDIR)/$(OUTPUT)
	chmod 755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): $(INCDIR)/ida.h $(INCDIR)/ida10.h $(INCDIR)/dbio.h $(INCDIR)/logio.h $(INCDIR)/util.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libida.a $(LIBDIR)/libida10.a $(LIBDIR)/libdbio.a $(LIBDIR)/liblogio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
