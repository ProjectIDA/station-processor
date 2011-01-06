# $Id: Makefile,v 1.6 2008/01/07 22:19:58 dechavez Exp $
# Make everything

all: lib bin
install: all

lib: FORCE
	date
	cd lib; $(MAKE)
	date

bin: FORCE
	date
	cd bin; $(MAKE)
	date

clean: FORCE
	cd lib; $(MAKE) $@
	cd bin; $(MAKE) $@

remove: FORCE
	rm -f $(PLATFORM).log Build.log
	cd lib; $(MAKE) REMOVE
	cd bin; $(MAKE) REMOVE

FORCE:
