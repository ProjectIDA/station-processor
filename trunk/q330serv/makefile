MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  LIB330_DIR = /home/q330dev/lib330
else
  LIB330_DIR = /usr/local/lib/lib330
endif
  LIB330_DIR = ../../../lib330

CC = gcc -g -Wl,--hash-style=both
COPT = 
LIBS = -L$(LIB330_DIR) -l330
CFLAGS = -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = daemonize.c callbacks.c commands.c config.c dump.c globals.c q330serv.c
SRCS+= logclient.c seedmsg.c ../dispstatus/statusutil.c

OBJS = $(SRCS:%.c=%.o)

q330serv : $(LIB330_DIR)/lib330.a $(OBJS) \
		../lib/libapi.a \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o q330serv -lz -lm -lpthread \
		../lib/libapi.a \
		../lib/dalib.a ../lib/dcctime2lib.a

q330serv.o: ../include/diskloop.h ../include/shmstatus.h

callbacks.o:	../include/diskloop.h ../include/q330arch.h ../include/shmstatus.h

../dispstatus/statusutil.o	: ../include/shmstatus.h
	cd ../dispstatus; $(MAKE)

$(LIB330_DIR)/lib330.a : 
	@echo
	@echo "lib330 has not been built, or can not be found in any"
	@echo "of the system library paths, or in $(LIB330_DIR)."
	@echo
	@exit -1

clean : 
		rm -f *.o q330serv

