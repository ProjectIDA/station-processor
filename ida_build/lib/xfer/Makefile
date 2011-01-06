# $Id: Makefile,v 1.6 2004/01/29 18:19:23 dechavez Exp $
# Makefile for xfer library

VPATH   = 
RANLIB  = ranlib
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
INCS    = -I$(INCDIR)
OPTMIZ  = -g
DEFS    = -D$(OSNAME) -D$(OSTYPE) -D$(MACHINE)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
ARCHIVE = libxfer.a
OUTPUT  = $(LIBDIR)/$(ARCHIVE)
OBJS    = \
         $(OUTPUT)(client.o) \
         $(OUTPUT)(common_r.o) \
         $(OUTPUT)(convert.o) \
         $(OUTPUT)(ezio.o) \
         $(OUTPUT)(globals.o) \
         $(OUTPUT)(log.o) \
         $(OUTPUT)(misc.o) \
         $(OUTPUT)(packet.o) \
         $(OUTPUT)(print.o) \
         $(OUTPUT)(rdwr.o) \
         $(OUTPUT)(server.o) \
         $(OUTPUT)(site.o) \
         $(OUTPUT)(tocss.o) \
         $(OUTPUT)(togse.o) \
         $(OUTPUT)(tosac.o) \
         $(OUTPUT)(toseed.o)

.PRECIOUS : $(OUTPUT)

all: OBJS/$(PLATFORM) $(LIBDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ makestamp

$(OUTPUT): makestamp $(INCDIR)/xfer.h

makestamp: $(OBJS)
	$(RANLIB) $(OUTPUT)
	touch makestamp
 
clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o
 
remove: OBJS/$(PLATFORM) FORCE
	rm -f $(OUTPUT) OBJS/$(PLATFORM)/makestamp

REMOVE: OBJS FORCE
	rm -rf OBJS $(OUTPUT)

OBJS:
	mkdir $@

$(LIBDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

# For testing

LIBD = ../../$(LIBDIR)
INCD = ../../$(INCDIR)
LIBS = -L$(LIBD) -lxfer -lida -lutil $(SOCKLIBS)

test_ezio: ezio.c
	cd OBJS/$(PLATFORM); \
	$(CC) -g -o $@ ../../ezio.c -I$(INCD) $(DEFS) -DDEBUG_TEST $(LIBS)
 
FORCE:
