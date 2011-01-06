# $Id: Makefile,v 1.21 2008/10/10 22:47:24 dechavez Exp $
# Unix makefile for isid

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
DEFS   += -DISI_SERVER
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS) $(EWFLAGS)

LIBS    = -L$(LIBDIR)
LIBS   += -lmsgq -lisidl -lnrts -lliss -lida -lisidb -lisi -liacp -llogio -lcssio -lida10 -ldbio -lutil
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += client.o
OBJS += convert.o
OBJS += copy.o
OBJS += die.o
OBJS += edes.o
OBJS += init.o
OBJS += isi.o
OBJS += hbeat.o
OBJS += log.o
OBJS += nrts.o
OBJS += par.o
OBJS += report.o
OBJS += respond.o
OBJS += service.o
OBJS += seqno.o
OBJS += signals.o
OBJS += status.o
OBJS += twind.o

OUTPUT  = isid
 
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

$(OBJS): isid.h $(INCDIR)/isi.h $(INCDIR)/util.h $(INCDIR)/msgq.h $(INCDIR)/logio.h $(INCDIR)/nrts.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libiacp.a $(LIBDIR)/libmsgq.a $(LIBDIR)/libutil.a $(LIBDIR)/liblogio.a $(LIBDIR)/libnrts.a $(LIBDIR)/libcssio.a $(LIBDIR)/libida.a $(LIBDIR)/libisidb.a $(LIBDIR)/libisidl.a $(LIBDIR)/libida10.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
