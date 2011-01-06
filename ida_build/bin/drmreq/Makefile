# $Id: Makefile,v 1.3 2008/01/21 23:10:43 dechavez Exp $
# Unix makefile for drmreq

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
DEFS   += -D_POSIX_PTHREAD_SEMANTICS
DEFS   += -DDEFAULT_SERVER=\"localhost\"
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(EWFLAGS)

LIBS    = -L$(LIBDIR) -lisi -lida -lliss -lisidb -lida10 -ldbio -lcssio -liacp -llogio -lutil
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SQLLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o

OUTPUT  = drmreq
 
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

$(OBJS): $(INCDIR)/isi.h $(INCDIR)/cssio.h $(INCDIR)/iacp.h $(INCDIR)/logio.h $(INCDIR)/util.h 
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libcssio.a $(LIBDIR)/libiacp.a $(LIBDIR)/liblogio.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
