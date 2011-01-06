# $Id: Makefile,v 1.5 2008/08/20 20:47:26 dechavez Exp $
# Unix makefile for iboot

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS    = -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR)
LIBS   += -liboot
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = iboot
 
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

$(OBJS): $(INCDIR)/iboot.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libiboot.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
