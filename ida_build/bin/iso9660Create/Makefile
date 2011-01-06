# $Id: Makefile,v 1.1 2008/03/10 20:50:05 dechavez Exp $
# Unix makefile for iso9660Create

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR)
LIBS   += -liso9660
LIBS   += -lutil

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = iso9660Create
 
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
	chmod 4755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): $(INCDIR)/iso9660.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libiso9660.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
