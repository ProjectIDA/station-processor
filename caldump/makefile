PLATFORM = $(shell uname)

CC = gcc -g

LIB330_DIR=../lib330
COPT =
LIBS =
CFLAGS = -Wall -B static -I. -I$(LIB330_DIR)

# Platform Specifics
ifeq ($(PLATFORM), SunOS)
    LIBS += -lsocket
endif

SRCS = caldump.c record.c dump_hex.c jio.c

OBJS = $(SRCS:%.c=%.o)

all: caldump

caldump: $(OBJS)
	$(CC) $(OBJS) ../lib330/lib330.a $(LIBS) -o caldump

clean:
	rm -f *.o caldump

