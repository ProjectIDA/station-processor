# $Id: Makefile,v 1.19 2008/03/05 23:23:48 dechavez Exp $
# Unix makefile for isidl

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
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)


LIBS    = -L$(LIBDIR)
LIBS   += -lisidl
LIBS   += -lnrts
LIBS   += -lida
LIBS   += -lisidb
LIBS   += -ldbio
LIBS   += -lisi
LIBS   += -lida10
LIBS   += -lcssio
LIBS   += -lliss
LIBS   += -lparo
LIBS   += -liacp
LIBS   += -lq330
LIBS   += -lqdplus
LIBS   += -lqdp
LIBS   += -lseed
LIBS   += -lttyio
LIBS   += -llogio
LIBS   += -lutil
LIBS   += -lmd5
LIBS   += -ludpio
LIBS   += -lmsgq
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += ars.o
OBJS += baro.o
OBJS += bground.o
OBJS += clock.o
OBJS += exit.o
OBJS += help.o
OBJS += ida5.o
OBJS += ida9.o
OBJS += ida10.o
OBJS += init.o
OBJS += intercept.o
OBJS += isi.o
OBJS += liss.o
OBJS += local.o
OBJS += log.o
OBJS += meta.o
OBJS += mseed.o
OBJS += q330.o
OBJS += qdplus.o
OBJS += rt593.o
OBJS += scl.o
OBJS += signals.o

OUTPUT  = isidl
 
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

$(OBJS): isidl.h $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
