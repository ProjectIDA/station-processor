# $Id: Makefile,v 1.17 2007/11/01 21:35:35 dechavez Exp $
# Make almost all NRTS executables and required libraries

LIB_SRCDIR = ../../lib

all: libs $(TARGET)

libs: FORCE
	cd $(LIB_SRCDIR); $(MAKE) cssio ida mtio ttyio util nrts xfer

solaris.sun4: FORCE
	cd alpha;     $(MAKE)
	cd edes;      $(MAKE)
	cd mon;       $(MAKE)
	cd utils;     $(MAKE)
	cd wrtdl;     $(MAKE)

openbsd.i86pc: FORCE
	cd edes;      $(MAKE)
	cd mon;       $(MAKE)
	cd utils;     $(MAKE)
	cd wrtdl;     $(MAKE)

freebsd.i86pc: FORCE
	cd edes;      $(MAKE)
	cd mon;       $(MAKE)
	cd utils;     $(MAKE)
	cd wrtdl;     $(MAKE)

linux.i86pc: FORCE
	cd edes;      $(MAKE)
	cd mon;       $(MAKE)
	cd utils;     $(MAKE)
	cd wrtdl;     $(MAKE)

FORCE:

install: libs $(TARGET)
#	cd alpha;     $(MAKE) $@
	cd edes;      $(MAKE) $@
	cd mon;       $(MAKE) $@
	cd utils;     $(MAKE) $@
	cd wrtdl;     $(MAKE) $@

clean:
	cd alpha;     $(MAKE) $@
	cd edes;      $(MAKE) $@
	cd mon;       $(MAKE) $@
	cd push;      $(MAKE) $@
	cd utils;     $(MAKE) $@
	cd wrtdl;     $(MAKE) $@
	cd xfer;      $(MAKE) $@

REMOVE:
	cd alpha;     $(MAKE) $@
	cd edes;      $(MAKE) $@
	cd mon;       $(MAKE) $@
	cd push;      $(MAKE) $@
	cd utils;     $(MAKE) $@
	cd wrtdl;     $(MAKE) $@
	cd xfer;      $(MAKE) $@

remove:
	cd alpha;     $(MAKE) $@
	cd edes;      $(MAKE) $@
	cd mon;       $(MAKE) $@
	cd push;      $(MAKE) $@
	cd utils;     $(MAKE) $@
	cd wrtdl;     $(MAKE) $@
	cd xfer;      $(MAKE) $@

FORCE:
