# $Id: Makefile,v 1.1 2007/01/19 17:48:22 dechavez Exp $
# Unix makefile for ida9to8

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS   += -lz
#LIBS   += -lm
#LIBS   += $(MTLIBS)
#LIBS   += $(POSIX4LIB)
#LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = ida9to8
 
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
 
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
