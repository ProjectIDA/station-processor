# $Id: Makefile,v 1.4 2005/09/13 00:36:37 dechavez Exp $
# Makefile for isp_logpkt

VPATH  =
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
DEFS   = -D$(OSNAME) -D$(OSTYPE)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
LIBS   = -lutil $(MTLIBS)
OBJS   = main.o

OUTPUT = isp_logpkt

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd $(BINDIR); rm -f $(OUTPUT)
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)

doinstall: $(OUTPUT)
	$(INSTALL) -c -m 755 $(OUTPUT) $(BINDIR)/$(OUTPUT)

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

FORCE:
