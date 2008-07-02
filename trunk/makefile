INSTALLDIR=$(HOME)/bin/
all:
	cd lib; $(MAKE)
	cd dispstatus; $(MAKE)
	cd q330serv; $(MAKE)
	cd ida2chan; $(MAKE)
	cd netreq; $(MAKE)
	cd dlutil; $(MAKE)
	cd log330; $(MAKE)
	cd q330arch; $(MAKE)
	cd ida2liss; $(MAKE)
	cd dispstatus; $(MAKE)

clean:
	cd lib; $(MAKE) clean
	cd q330serv; $(MAKE) clean
	cd ida2chan; $(MAKE) clean
	cd netreq; $(MAKE) clean
	cd aslreQ; $(MAKE) clean
	cd dlutil; $(MAKE) clean
	cd log330; $(MAKE) clean
	cd q330arch; $(MAKE) clean
	cd ida2liss; $(MAKE) clean
	cd dispstatus; $(MAKE) clean

install:
	cp -p q330serv/q330serv ida2chan/ida2chan netreq/netreq dlutil/dlutil \
     log330/log330 q330arch/q330arch ida2liss/ida2liss dispstatus/dispstatus \
      $(INSTALLDIR)
cpio:
	find q330serv ida2chan aslreq netreq dispstatus dlutil log330 q330arch ida2liss lib include -name '*.[ch]' -print >/tmp/flist
	find . -name '[mM]akefile' -print >>/tmp/flist
	sort /tmp/flist >/tmp/slist	
	cpio -ov </tmp/slist >q330asp.cpio
