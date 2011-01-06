# $Id: Makefile,v 1.9 2006/06/02 21:14:30 dechavez Exp $
# Makefile for mon

VPATH  = 
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)

CFLAGS = $(OPTMIZ) $(INCS) -D$(OSNAME) -D$(OSTYPE)
LIBS   = $(CURSELIB) -L$(LIBDIR) -lnrts -lida -lutil -llogio $(SOCKLIBS) $(MTLIBS)

OBJS   = main.o
OBJS  += mkscreen.o
OBJS  += log_win.o
OBJS  += stat_win.o
OBJS  += die.o
OBJS  += chpid.o
OBJS  += help_menu.o
OBJS  += do_cmd.o
OBJS  += chlog.o
OBJS  += chsystem.o
OBJS  += systems.o 
OBJS  += getmaxnchn.o

OUTPUT = nrts_mon

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

$(OBJS): nrts_mon.h $(INCDIR)/nrts.h

$(OUTPUT): $(OBJS) $(LIBDIR)/libnrts.a $(LIBDIR)/libida.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
