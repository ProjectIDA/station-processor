# DCC makefile
#
#
include $(DCC_INCLUDE)/dcc_make.mk
DCC_BIN=/home/fshelly/bin/

INCS+= -I../lib330
INCS+= -I.

TFILS= 

PROF=-g

OFADUMP_SRCS = ofadump.c record.c format_data.c jio.c fmash.c \
               falcon_structures.c buffer.c simclist.c
OFADUMP_OBJS = $(OFADUMP_SRCS:%.c=%.o)

all: falconalarm ofadump falconweb mergeopaque

falconalarm:	falconalarm.o traversetr1.o $(FMTS)
	$(CC) $(STRTUP) -o falconalarm  \
                falconalarm.o traversetr1.o $(DCC_SEED) \
                $(DCC_LIBS)

ofadump:	$(OFADUMP_OBJS) $(FMTS)
	$(CC) $(STRTUP) -o ofadump  \
		$(OFADUMP_OBJS) ../lib330/lib330.a \
                $(DCC_LIBS)

mergeopaque:	mergeopaque.o $(FMTS)
	$(CC) $(STRTUP) -o mergeopaque  \
		mergeopaque.o \
                $(DCC_LIBS)

falconweb:      falconweb.o
	$(CC) $(STRTUP) -o falconweb  \
		falconweb.o \
                $(DCC_LIBS)

ofadump.o:	fmash.h falcon_structures.h format_data.h jio.h record.h tokens.h buffer.h falcon.h simclist.h

fmash.o:	fmash.h simclist.h

install:	falconalarm falconweb ofadump mergeopaque
	/bin/ls falconalarm falconweb ofadump mergeopaque | cpio -pvm $(DCC_BIN)

nice:
	rm -f core *~ $(TFILS)

clean:	
	rm -f core *.o *~ $(TFILS)
