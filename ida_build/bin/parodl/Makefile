# $Id: Makefile,v 1.1 2009/03/24 21:11:40 dechavez Exp $
# Unix makefile for parodl

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


LIBS    = -L$(LIBDIR)
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lisi
LIBS   += -lida10
LIBS   += -lliss
LIBS   += -lparo
LIBS   += -liacp
LIBS   += -lttyio
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += baro.o
OBJS += bground.o
OBJS += dl.o
OBJS += exit.o
OBJS += log.o
OBJS += packet.o
OBJS += signals.o

OUTPUT  = parodl
 
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

$(OBJS): parodl.h $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a $(LIBDIR)/libida10.a $(LIBDIR)/libparo.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
