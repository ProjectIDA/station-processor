# $Id: Makefile,v 1.33 2008/08/21 21:28:56 dechavez Exp $
# Makefile for ISP daemon

VPATH  =
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
NOTESDIR= $(VPATH)../../../ReleaseNotes
RELEASENOTES = $(VPATH)../../../lib/isp/globals.c
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS   += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
LIBS   = -lisp
LIBS  += -loldmsgq
LIBS  += -lisidl
LIBS  += -lnrts
LIBS  += -lliss
LIBS  += -lxfer
LIBS  += -lida
LIBS  += -lisidb
LIBS  += -lisi
LIBS  += -liacp
LIBS  += -llogio
LIBS  += -loldparo
LIBS  += -lsclk
LIBS  += -lseed
LIBS  += -lcssio
LIBS  += -loldttyio
LIBS  += -lida10
LIBS  += -ldbio
LIBS  += -lsanio
LIBS  += -liboot
LIBS  += -lutil
LIBS  += -llogio
LIBS  += -lz
LIBS  += -lm
LIBS  += $(MTLIBS)
LIBS  += $(SOCKLIBS)
LIBS  += $(SQLLIBS)

OBJS   = main.o
OBJS  += auxdata.o
OBJS  += baro.o
OBJS  += cdio.o
OBJS  += clock.o
OBJS  += config.o
OBJS  += console.o
OBJS  += dasio.o
OBJS  += daslog.o
OBJS  += dasops.o
OBJS  += decode.o
OBJS  += die.o
OBJS  += dpm.o
OBJS  += flags.o
OBJS  += flushcf.o
OBJS  += ida8.o
OBJS  += ida10.o
OBJS  += init.o
OBJS  += inject.o
OBJS  += iboot.o
OBJS  += isi.o
OBJS  += log.o
OBJS  += massio.o
OBJS  += mediaops.o
OBJS  += msgq.o
OBJS  += patch.o
OBJS  += process.o
OBJS  += request.o
OBJS  += restart.o
OBJS  += sanio.o
OBJS  += sanlog.o
OBJS  += sanops.o
OBJS  += signals.o
OBJS  += soh.o
OBJS  += status.o
OBJS  += tapeio.o

OUTPUT = ispd

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd $(BINDIR); rm -f $(OUTPUT)
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 ispd $(BINDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

notes: $(RELEASENOTES)
	$(INSTALL) -c -m 664 $(RELEASENOTES) $(NOTESDIR)/isp

FORCE:

$(OBJS): $(INCDIR)/isp.h ispd.h

ispd: $(OBJS) $(LIBDIR)/libisp.a $(LIBDIR)/libsanio.a $(LIBDIR)/libutil.a $(LIBDIR)/libisi.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

test_baro: baro.c msgq.o
	$(CC) $(CFLAGS) -DDEBUG_TEST -D$(MACHINE) -o OBJS/$(PLATFORM)/$@ baro.c msgq.o $(LIBS)
