INSTALLDIR=$(HOME)/bin/
all:
	cd lib; $(MAKE)
	cd dispstatus; $(MAKE)
	cd q330serv; $(MAKE)
	cd ida2arch; $(MAKE)
	cd ida2chan; $(MAKE)
	cd ida2liss; $(MAKE)
	cd isirstat; $(MAKE)
	cd netreq; $(MAKE)
	cd dlresize; $(MAKE)
	cd dlutil; $(MAKE)
	cd log330; $(MAKE)
	cd q330arch; $(MAKE)
	cd dispstatus; $(MAKE)
	cd netseed; $(MAKE)
	cd falcon; $(MAKE)
	cd shearreq; $(MAKE)
    cd ocf_utils; $(MAKE)
    cd caldump; $(MAKE)

clean:
	cd lib; $(MAKE) clean
	cd q330serv; $(MAKE) clean
	cd ida2arch; $(MAKE) clean
	cd ida2chan; $(MAKE) clean
	cd ida2liss; $(MAKE) clean
	cd isirstat; $(MAKE) clean
	cd netreq; $(MAKE) clean
	cd aslreq; $(MAKE) clean
	cd dlutil; $(MAKE) clean
	cd dlresize; $(MAKE) clean
	cd log330; $(MAKE) clean
	cd q330arch; $(MAKE) clean
	cd dispstatus; $(MAKE) clean
	cd netseed; $(MAKE) clean
	cd falcon; $(MAKE) clean
	cd shearreq; $(MAKE) clean
	cd shake; $(MAKE) clean
    cd ocf_utils; $(MAKE) clean
    cd caldump; $(MAKE) clean

outside:
	cd lib330; $(MAKE)
	cd ida_build; $(MAKE)

shakeit:
	cd shake; $(MAKE)
install:
	cp -p q330serv/q330serv ida2chan/ida2chan netreq/netreq dlutil/dlutil \
     log330/log330 q330arch/q330arch ida2liss/ida2liss \
     dispstatus/dispstatus netseed/netseed ida2arch/ida2arch isirstat/isirstat \
     falcon/falcon dlresize/dlresize shearreq/shearreq \
      $(INSTALLDIR)
cpio:
	find aslreq q330serv ida2arch ida2chan isirstat netreq dispstatus dlresize dlutil log330 q330arch ida2liss netseed falcon shearreq lib include -name '*.[ch]' -print >/tmp/flist
	find . -name '[mM]akefile' -print >>/tmp/flist
	sort /tmp/flist >/tmp/slist	
	cpio -ov </tmp/slist >q330asp.cpio
