# Make seed library routines
# Needed to decompress seed records for aslreq program
#
MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC=gcc -g 
else
  CC=gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../../../lib330
CFLAGS=-I. -I../../include
DEFS = -DHAVE_CONFIG_H
SEEDLIB=seedlib.a
PROF = -O2
OBJS = steimlib.o
OBJS+= steim_decode.o

CC = gcc -g -O2
all:	$(SEEDLIB)

$(SEEDLIB): $(OBJS)
	$(AR) rv $@ $?

$(OBJS):

clean:
	rm -f *.o *.a core *~ $(SEEDLIB)

