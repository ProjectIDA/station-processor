# $Id: Makefile,v 1.9 2008/01/07 22:06:12 dechavez Exp $
# Unix makefile for isimkdl

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
DEFS   += $(SQLDEFS)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR) -lisidl -lnrts -lliss -lida -lisidb -ldbio -lisi -lida10 -liacp -llogio -lutil
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = isimkdl
 
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

$(OBJS): $(INCDIR)/isi.h $(INCDIR)/isi/dl.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisidl.a $(LIBDIR)/libisi.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
