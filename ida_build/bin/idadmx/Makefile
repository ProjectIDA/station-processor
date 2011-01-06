# $Id: Makefile,v 1.14 2007/01/05 00:47:54 dechavez Exp $
# Makefile for idadmx

VPATH  = 
INCDIR = $(VPATH)../../include
BINDIR = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) $(DEFS)
LIBS   = -L$(LIBDIR)
LIBS  += -lida
LIBS  += -lisidb
LIBS  += -ldbio
LIBS  += -lcssio
LIBS  += -lmio
LIBS  += -loldmtio
LIBS  += -lutil
LIBS  += -llogio
LIBS  += $(SQLLIBS)
LIBS  += $(SOCKLIBS)
LIBS  += $(MTLIBS)

OBJS   = ReleaseNotes.o
OBJS  += main.o
OBJS  += drec_ok.o
OBJS  += exitcode.o
OBJS  += fixyear.o
OBJS  += help.o
OBJS  += init.o
OBJS  += loadbuf.o
OBJS  += msglog.o
OBJS  += names.o
OBJS  += prthead.o
OBJS  += rdrec.o
OBJS  += wfdisc.o
OBJS  += wrtdat.o

OUTPUT = idadmx

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OUTPUT): $(OBJS) \
  $(LIBDIR)/libutil.a \
  $(LIBDIR)/libmio.a \
  $(LIBDIR)/liboldmtio.a \
  $(LIBDIR)/libcssio.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(INCDIR)/ida.h idadmx.h
