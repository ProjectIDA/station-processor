MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  LIB330_DIR = /home/q330dev/lib330
else
  LIB330_DIR = /usr/local/lib/lib330
endif
  LIB330_DIR = $(HOME)/lib330

CC = gcc
COPT = 
LIBS = -L$(LIB330_DIR) -l330
CFLAGS = -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = dispstatus.c
SRCS+= daemonize.c

OBJS = $(SRCS:%.c=%.o)

dispstatus : $(LIB330_DIR)/lib330.a $(OBJS) \
		../lib/libapi.a  \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o dispstatus -lz -lm -lpthread \
		../lib/libapi.a \
		../lib/dalib.a ../lib/dcctime2lib.a

dispstatus.o: ../include/diskloop.h

callbacks.o:	../include/diskloop.h ../include/q330arch.h

$(LIB330_DIR)/lib330.a : 
	@echo
	@echo "lib330 has not been built, or can not be found in any"
	@echo "of the system library paths, or in $(LIB330_DIR)."
	@echo
	@exit -1

clean : 
		rm -f *.o dispstatus

