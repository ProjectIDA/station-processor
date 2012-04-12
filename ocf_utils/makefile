PLATFORM = $(shell uname)
LIB330_DIR=../lib330
LIBPCOMM_DIR=../libpcomm

LIB_PATHS = -L$(LIB330_DIR) -L$(LIBPCOMM_DIR)
INCLUDES = -I. -I$(LIB330_DIR) -I$(LIBPCOMM_DIR)
COPT =
LIBS = -lm -lpthread -l330 -lpcomm
# Platform Magic
ifeq ($(PLATFORM), SunOS)
    LIBS += -lsocket
endif

CFLAGS = -Wall -B static $(INCLUDES) $(LIB_PATHS)

CC = gcc -g $(CFLAGS)

all: ocfdump ocfdiff

COMMON_SRCS = dump_hex.c jio.c record.c tokens.c

DUMP_SRCS = ocfdump.c $(COMMON_SRCS)
DUMP_OBJS = $(DUMP_SRCS:%.c=%.o)

DIFF_SRCS = ocfdiff.c $(COMMON_SRCS)
DIFF_OBJS = $(DIFF_SRCS:%.c=%.o)

all: ocfdump ocfdiff

ocfdump: $(DUMP_OBJS) libpcomm.a
	$(CC) $(DUMP_OBJS) $(LIBS) -o $@

ocfdiff: $(DIFF_OBJS) libpcomm.a
	$(CC) $(DUMP_OBJS) $(LIBS) -o $@

libpcomm.a: $(LIBPCOMM_DIR)/$@
	cd $(LIBPCOMM_DIR); $(MAKE)

.PHONY: clean
clean:
	rm -f *.o ocfdump ocfdiff

