
all: ocfdump ocfdiff

ocfdump: ./Makefile.ocfdump
	make -f $<;

ocfdiff: ./Makefile.ocfdiff
	make -f $<;

.PHONY: clean
clean:
	rm -f *.o ocfdump ocfdiff

