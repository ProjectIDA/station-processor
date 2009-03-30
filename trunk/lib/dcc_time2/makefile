# This is the Makefile for the DCC_TIME2 library.  It contains
# a subset of the DCC_TIME routines that have been renamed by
# appending a 2 to the end of the original name.  These routines
# use STDTIME2 structures and have tenth of a millisecond precision
#
CC=gcc -g -Wl,--hash-style=both
CFLAGS=-I../../include
DCC_TIME2=../dcctime2lib.a
PROF = -O2
OBJS = 	st_addtotime2.o \
	st_deltatoms2.o \
	st_getjulian2.o \
	st_minusdelta2.o \
	st_timenorm2.o \
	st_cleandate2.o \
	st_difftimes2.o \
	st_julian2.o \
	st_parsetime2.o \
	st_printdate2.o \
	st_deltaprint2.o \
	st_getcurrent2.o \
	st_minmax2.o \
	st_timecomp2.o


all:	$(DCC_TIME2)

$(DCC_TIME2): $(OBJS)
	$(AR) rv $@ $?

$(OBJS):


clean:
	rm -f *.o *.a core *~ $(DCC_TIME2)

