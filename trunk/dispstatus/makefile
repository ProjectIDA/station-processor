MACHINE = $(shell uname -m)
ifeq ($(MACHINE), armv5tel)
  CC = gcc -g 
else
  CC = gcc -g -Wl,--hash-style=both
endif

LIB330_DIR=../../../lib330
COPT = 
LIBS = -L$(LIB330_DIR) -l330
CFLAGS = -I.. -I. -I$(LIB330_DIR)
LIBS += -lm -lpthread

SRCS = dispstatus.c
SRCS+= daemonize.c
SRCS+= statusutil.c

OBJS = $(SRCS:%.c=%.o)

dispstatus : $(LIB330_DIR)/lib330.a $(OBJS) \
		../lib/dalib.a ../lib/dcctime2lib.a
	$(CC) $(OBJS) $(LIBS) -o dispstatus -lz -lm -lpthread \
		../lib/dalib.a ../lib/dcctime2lib.a

dispstatus.o: ../include/diskloop.h ../include/shmstatus.h

statusutil.o: ../include/shmstatus.h

$(LIB330_DIR)/lib330.a : 
	@echo
	@echo "lib330 has not been built, or can not be found in any"
	@echo "of the system library paths, or in $(LIB330_DIR)."
	@echo
	@exit -1

clean : 
		rm -f *.o dispstatus

