# DCC makefile
#
#
# remove next line for final installation
DCC_BIN=/home/fshelly/bin/

PROF= -O2

all: shearreq


shearreq: shearreq.o
	$(CC) -o shearreq shearreq.o -lm

install: shearreq
	cp shearreq $(DCC_BIN)

nice:
	rm -f core *~ shearreq

clean:	
	rm -f core *.o shearreq
