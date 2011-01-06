# $Id: Makefile,v 1.21 2008/01/21 23:10:41 dechavez Exp $
# Makefile for DRM programs

VPATH   =
BINDIR  = $(VPATH)../../../bin/$(PLATFORM)
LIBDIR  = $(VPATH)../../../lib/$(PLATFORM)
INCDIR  = $(VPATH)../../include
OPTMIZ  = -g
INCS    = -I$(INCDIR)
INCS   += -I/opt/local/include
INCS   += $(SQLINCS)
DEFS    = -D$(OSNAME) -D$(OSTYPE)
INCS   += $(SQLDEFS)
CFLAGS  = $(OPTMIZ) $(INCS) $(DEFS)
LIBS    = -L$(LIBDIR)
LIBS   += -L/opt/local/lib
LIBS   += -lxfer
LIBS   += -lida10
LIBS   += -lida
LIBS   += -lqdp
LIBS   += -lisidb
LIBS   += -lisidl
LIBS   += -lisi
LIBS   += -lliss
LIBS   += -ldbio
LIBS   += -lcssio
LIBS   += -lsacio
LIBS   += -lseed
LIBS   += -liacp
LIBS   += -lutil
LIBS   += -llogio
LIBS   += -lz
LIBS   += -lm
LIBS   += $(SQLLIBS)
LIBS   += $(SOCKLIBS)

LIBRARIES  = $(LIBDIR)/libxfer.a \
LIBRARIES += $(LIBDIR)/liblogio.a \
LIBRARIES += $(LIBDIR)/libisi.a \
LIBRARIES += $(LIBDIR)/libiacp.a \
LIBRARIES += $(LIBDIR)/libida.a \
LIBRARIES += $(LIBDIR)/libida10.a \
LIBRARIES += $(LIBDIR)/libcssio.a \
LIBRARIES += $(LIBDIR)/libsacio.a \
LIBRARIES += $(LIBDIR)/libseed.a \
LIBRARIES += $(LIBDIR)/libutil.a

OBJS   = main.o
OBJS  += abuser.o
OBJS  += ack.o
OBJS  += auth.o
OBJS  += avail.o
OBJS  += canned.o
OBJS  += chanlst.o
OBJS  += convert.o
OBJS  += datreq.o
OBJS  += deliver.o
OBJS  += email.o
OBJS  += exit.o
OBJS  += fnmatch.o
OBJS  += ftp.o
OBJS  += getcnf.o
OBJS  += getinfo.o
OBJS  += gzip.o
OBJS  += init.o
OBJS  += isidata.o
OBJS  += merge.o
OBJS  += names.o
OBJS  += prep.o
OBJS  += rcp.o
OBJS  += rdautodrm.o
OBJS  += rdcnf.o
OBJS  += rdfile.o
OBJS  += rdidadrm.o
OBJS  += rdspyder.o
OBJS  += remap.o
OBJS  += service.o
OBJS  += split.o
OBJS  += xferdata.o

OUTPUT = drmd drm_spool drm_check drm_service drm_makeinfo

all: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ $(OUTPUT)

install: OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd OBJS/$(PLATFORM); \
	$(MAKE) -f ../../Makefile VPATH=../../ doinstall

clean: OBJS/$(PLATFORM) FORCE
	rm -f OBJS/$(PLATFORM)/*.o

remove:  OBJS/$(PLATFORM) $(BINDIR) FORCE
	cd $(BINDIR); rm -f $(OUTPUT)
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT)

doinstall: drmd drm_spool drm_service drm_check drm_makeinfo
	/bin/rm -f $(BINDIR)/drmd
	/bin/rm -f $(BINDIR)/drm_spool
	/bin/rm -f $(BINDIR)/drm_check
	/bin/rm -f $(BINDIR)/drm_service
	/bin/rm -f $(BINDIR)/drm_makeinfo
	/bin/cp drmd $(BINDIR)
	/bin/cp drm_spool $(BINDIR)
	/bin/cp drm_check $(BINDIR)
	/bin/cp drm_service $(BINDIR)
	/bin/cp drm_makeinfo $(BINDIR)
	/bin/chmod 4755 $(BINDIR)/drmd $(BINDIR)/drm_spool

rebuild:
	$(MAKE) clean remove all

relink:
	cd OBJS/$(PLATFORM); rm -f $(OUTPUT); cd ../..; \
	$(MAKE) all

reinstall:
	$(MAKE) remove install

REMOVE: $(BINDIR)
	rm -rf OBJS
	cd $(BINDIR); rm -f $(OUTPUT)

$(BINDIR):
	mkdir -p $@

OBJS/$(PLATFORM):
	mkdir -p $@

FORCE:

names.o: $(VPATH)names.c drm.h
rdcnf.o: $(VPATH)rdcnf.c drm.h

$(OBJS): drm.h service.h
spool.o: drm.h

drmd.o: $(VPATH)drmd.c drm.h
	$(CC) $(CFLAGS) -c -o $@ $(VPATH)drmd.c

drmd: drmd.o rdcnf.o names.o getinfo.o \
    $(LIBDIR)/libutil.a drm.h
	$(CC) $(CFLAGS) -o $@ drmd.o rdcnf.o names.o getinfo.o -L$(LIBDIR) -lutil -llogio $(SOCKLIBS) $(MTLIBS)

drm_spool: spool.o names.o getinfo.o rdcnf.o \
    $(LIBDIR)/libutil.a drm.h
	$(CC) $(CFLAGS) -o $@ spool.o names.o rdcnf.o getinfo.o -L$(LIBDIR) -lutil -llogio $(SOCKLIBS) $(MTLIBS)

drm_check: check.o names.o rdcnf.o avail.o datreq.o getinfo.o drm.h \
    $(LIBDIR)/libutil.a drm.h
	$(CC) $(CFLAGS) -o $@ check.o names.o rdcnf.o datreq.o getinfo.o -L$(LIBDIR) -lutil -llogio $(SOCKLIBS) $(MTLIBS)

drm_service: $(OBJS) drm.h service.h
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(SOCKLIBS) -llogio $(MTLIBS)

drm_makeinfo: makeinfo.o getinfo.o names.o rdcnf.o $(LIBDIR)/libutil.a $(LIBDIR)/libxfer.a
	$(CC) $(CFLAGS) -o $@ makeinfo.o names.o rdcnf.o getinfo.o $(LIBS) $(MTLIBS)
