# $Id: Makefile,v 1.14 2006/02/10 02:23:42 dechavez Exp $
# Makefile for isp operator interface

VPATH  =
INCDIR = $(VPATH)../../../include
BINDIR = $(VPATH)../../../../bin/$(PLATFORM)
LIBDIR = $(VPATH)../../../../lib/$(PLATFORM)
OPTMIZ = -g
INCS   = -I$(INCDIR)
INCS  += $(SQLINCS)
DEFS   = -D$(OSNAME) -D$(OSTYPE) -D_REENTRANT
DEFS  += $(SQLDEFS)
CFLAGS = $(OPTMIZ) $(INCS) -L$(LIBDIR) $(DEFS)
LIBS   = -lisp -lnrts -lida10 -loldttyio -lida -lisidb -ldbio -lutil -llogio
LIBS  += $(MTLIBS)
LIBS  += $(SOCKLIBS)
LIBS  += $(CURSELIB)
LIBS  += $(SQLLIBS)

OBJS   = main.o
OBJS  += beep.o
OBJS  += calib.o
OBJS  += cmddas.o
OBJS  += cmdsan.o
OBJS  += cnfdas.o
OBJS  += dispdas.o
OBJS  += dispgen.o
OBJS  += dispsan.o
OBJS  += reboot.o
OBJS  += save.o
OBJS  += signals.o
OBJS  += socket.o
OBJS  += status.o
OBJS  += viewlog.o

OUTPUT = isp

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

$(OBJS): $(INCDIR)/isp.h $(VPATH)isp_console.h

$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# For testing
 
TEST_LIBD = ../../$(LIBDIR)
TEST_INCD = ../../$(INCDIR)
TEST_LIBS = -L$(TEST_LIBD) -lisp -lutil $(CURSELIB) $(SOCKLIBS)
 
test_curse: curse.c
	cd OBJS/$(PLATFORM); \
	$(CC) -g -o $@ ../../curse.c -I$(TEST_INCD) $(DEFS) -DDEBUG_TEST $(TEST_LIBS)
 
test_calib: calib.c
	cd OBJS/$(PLATFORM); \
	$(CC) -g -o $@ ../../calib.c -I$(TEST_INCD) $(DEFS) -DDEBUG_TEST $(TEST_LIBS)
 
FORCE:
