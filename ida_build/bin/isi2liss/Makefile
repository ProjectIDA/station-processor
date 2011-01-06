# $Id: Makefile,v 1.6 2008/02/01 23:09:49 dechavez Exp $
# Unix makefile for isi2liss

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
DEFS   += -DINCLUDE_Q330
CPPFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR)
LIBS   += -lstdc++
LIBS   += -lisi
LIBS   += -lida
LIBS   += -lliss
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lcssio
LIBS   += -liacp
LIBS   += -llogio
LIBS   += -lida10
LIBS   += -lttyio
LIBS   += -lutil
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

LOCALINCS  = ISICLIENT_PARAM.h
LOCALINCS += RawDataBlock.h
LOCALINCS += ServerThread.h
LOCALINCS += ServerThreadPool.h

OBJS  = isi2liss.o
OBJS += ReleaseNotes.o
OBJS += isiclient.o
OBJS += RawDataBlock.o
OBJS += ServerThread.o
OBJS += ServerThreadPool.o
OBJS += ISICLIENT_PARAM.o

OUTPUT  = isi2liss
 
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

$(OBJS): $(LOCALINCS) $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
