# $Id: Makefile,v 1.5 2006/02/10 02:26:43 dechavez Exp $
# Make all ISP executables and required libraries

LIB_SRCDIR = ../../lib
LIBS       = isp paro sclk nrts cssio ttyio msgq mtio rdio util isidl ida isidb dbio ida10

all: libs progs

libs: FORCE
	cd $(LIB_SRCDIR); $(MAKE) $(LIBS)

progs: FORCE
	cd bootmgr; $(MAKE)
	cd inject ; $(MAKE)
	cd display; $(MAKE)
	cd ispd;    $(MAKE)
	cd logpkt ; $(MAKE)

FORCE:

install: libs progs
	cd bootmgr; $(MAKE) $@
	cd inject;  $(MAKE) $@
	cd display; $(MAKE) $@
	cd ispd;    $(MAKE) $@
	cd logpkt;  $(MAKE) $@

clean:
	cd bootmgr; $(MAKE) $@
	cd inject;  $(MAKE) $@
	cd display; $(MAKE) $@
	cd ispd;    $(MAKE) $@
	cd logpkt;  $(MAKE) $@

remove:
	cd bootmgr; $(MAKE) $@
	cd inject;  $(MAKE) $@
	cd display; $(MAKE) $@
	cd ispd;    $(MAKE) $@
	cd logpkt;  $(MAKE) $@

REMOVE:
	cd bootmgr; $(MAKE) $@
	cd inject;  $(MAKE) $@
	cd display; $(MAKE) $@
	cd ispd;    $(MAKE) $@
	cd logpkt;  $(MAKE) $@
