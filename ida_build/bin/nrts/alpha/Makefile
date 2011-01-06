# $Id: Makefile,v 1.13 2008/09/11 16:40:14 dechavez Exp $
# Makefile for nrts_alpha

VPATH    = 
NOTESDIR = $(VPATH)../../../ReleaseNotes

# Use this one to test the developmental libdss
#ALPHADIR   = /home/alpha/alpha-1.3A/$(PLATFORM)
# -OR- use this one for the "stable" release of libalpha
ALPHADIR   = $(BUILDHOME)/3rdParty/alpha-1.3/$(PLATFORM)

ALPHAIDIR  = $(ALPHADIR)/include
ALPHALDIR  = $(ALPHADIR)/lib
ALPHALIBS  = -L$(ALPHALDIR) -lalpha -lcancomp -lconvert -lheap -lmisc
ALPHALIBS += -lsmalloc -llogout -lpar

# Define ThirdPartyCryptoStuff appropriate for this build

# Uncomment this to not use any crypto cards
TPCSDEFS  =
TPCSDIR   =
TPCSINCS  =
TPCSLIBS  =

# -OR- Uncomment this to use Spyrus Fortezza card
#TPCSDEFS  = -DUSE_LYNKS_LIBDSS
#TPCSDIR   = /opt/LYNKSees
#TPCSINCS  = -I$(TPCSDIR)/include
#TPCSLIBS  = -L$(TPCSDIR)/lib -llynksees
#ALPHALIBS += -ldss

# -OR- Uncomment this to use Chrysalis Luna 2
#TPCSDEFS  = -DUSE_LUNA_LIBDSS -DUNIX -DSPARC -DCRYPTOKI_201
#TPCSDIR   = /usr/luna
#TPCSINCS  = -I$(TPCSDIR)/samples/source
#TPCSLIBS  = -L$(TPCSDIR)/lib -lcrystoki_gcc2
#ALPHALIBS += -ldss

INCDIR   = $(VPATH)../../../include
BINDIR   = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR   = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ   = -g

INCS    = -I$(INCDIR) -I$(ALPHAIDIR) $(TPCSINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE) $(TPCSDEFS)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
LIBS    = -L$(LIBDIR) -lnrts -lisi -lida10 -lida -lliss -lisidb -lisidl -ldbio -lseed -lutil -llogio
LIBS   += $(ALPHALIBS)
LIBS   += $(TPCSLIBS)
LIBS   += -lm
LIBS   += $(SOCKLIBS)
LIBS   += $(SQLLIBS)

OBJS   = main.o ReleaseNotes.o init.o exitcode.o read.o

OUTPUT = nrts_alpha

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall notes

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	rm -f OBJS/$(PLATFORM)/$(OUTPUT) $(BINDIR)/$(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)

notes: $(VPATH)ReleaseNotes
	$(INSTALL) -c -m 664 $(VPATH)ReleaseNotes $(NOTESDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

$(OBJS): $(INCDIR)/nrts.h nrts_alpha.h

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
