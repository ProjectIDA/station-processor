# $Id: Makefile,v 1.4 2008/02/01 18:23:44 akimov Exp $
# Unix makefile for auz_isi

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
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
LIBS   += -lida10
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

LOCALINCS  = AUZEvent.h
LOCALINCS += Average.h
LOCALINCS += EvInfo.h
LOCALINCS += IIRfilter.h
LOCALINCS += auzglobals.h
LOCALINCS += auzinfo.h
LOCALINCS += detect.h
LOCALINCS += detector.h
LOCALINCS += mail.h

OBJS  = ReleaseNotes.o
OBJS += auz_isi.o
OBJS += AUZEvent.o
OBJS += auzglobals.o
OBJS += AVERAGE.o
OBJS += detect.o
OBJS += detector.o
OBJS += dft.o
OBJS += EvInfo.o
OBJS += IIRfilter.o
OBJS += mail.o
OBJS += Trend.o

OUTPUT  = auz_isi
 
all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	make -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
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
 
$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OBJS): $(LOCALINCS) $(INCDIR)/isi.h $(INCDIR)/nrts.h $(INCDIR)/util.h $(INCDIR)/ttyio.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libisi.a $(LIBDIR)/libnrts.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
