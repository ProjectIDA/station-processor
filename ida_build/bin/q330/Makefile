# $Id: Makefile,v 1.9 2009/02/04 17:51:31 dechavez Exp $
# Unix makefile for q330

VPATH   = 
INCDIR  = $(VPATH)../../include
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)

OPTMIZ  = -g
INCS   += -I$(INCDIR)
DEFS    = -D$(OSNAME) -D$(OSTYPE) -DOSVER=$(OSVER) -D_REENTRANT
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)

LIBS    = -L$(LIBDIR) -lq330 -lqdp -lida10 -lmsgq -ludpio -lutil -llogio -lmd5 -lm
LIBS   += $(MTLIBS)
LIBS   += $(SOCKLIBS)

OBJS  = ReleaseNotes.o
OBJS += main.o
OBJS += amass.o
OBJS += calib.o
OBJS += checkout.o
OBJS += cmd.o
OBJS += dcp.o
OBJS += fixvar.o
OBJS += glob.o
OBJS += gps.o
OBJS += ifconfig.o
OBJS += init.o
OBJS += man.o
OBJS += pulse.o
OBJS += reboot.o
OBJS += resync.o
OBJS += save.o
OBJS += sc.o
OBJS += spp.o
OBJS += status.o

OUTPUT  = q330
 
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
	chmod 4755 $(BINDIR)/$(OUTPUT)
 
OBJS/$(PLATFORM):
	mkdir -p OBJS/$(PLATFORM)

FORCE:

$(OBJS): q330cc.h $(INCDIR)/q330.h
 
$(OUTPUT): $(OBJS) $(LIBDIR)/libqdp.a $(LIBDIR)/libutil.a
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
