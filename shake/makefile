#=============================================================================#
# Makefile for:  shake table data collection software                         #
#=============================================================================#

#-----------------------------------------------------------------------------#
#  1.  Specify locations of things on system         ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#

# These local definitions will be overridden by superior makefiles
PROJECT        = .
PROJECT_INCDIR = .
PROJECT_LIBDIR = .
PROJECT_BINDIR = .
PROJECT_MACDIR = .

# Redefine these if installables go to other than the project-standard places
OUTPUT_INCDIR  = $(PROJECT_INCDIR)
OUTPUT_LIBDIR  = $(PROJECT_LIBDIR)
OUTPUT_BINDIR  = $(PROJECT_BINDIR)



#-----------------------------------------------------------------------------#
#  2.  Specify products of this directory            ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#

EXEC_OUTPUT    = ufors
LIB_OUTPUT     = 
INC_OUTPUT     = 
CLEAN          = $(OBJECTS) *~ *.bak core            # make clean rm ...



#-----------------------------------------------------------------------------#
#  3.  Specify sources, objects, defines             ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#
# SOURCES, HEADERS, and OBJECTS must be defined if any exist

SOURCES = *.c

OBJECTS = \
daemonize.o \
genutils.o \
gps.o \
gswap.o \
lmplatform.o \
logclient.o \
logging.o \
lookup.o \
msrutils.o \
openraw.o \
pack.o \
packdata.o \
seedmsg.o \
ufors.o

HEADERS = *.h

DEFINES =



#-----------------------------------------------------------------------------#
#  4.  Specify libraries to be linked to executables ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#

MLIBS          = ../lib/dalib.a ../lib/dcctime2lib.a          # personal
PLIBS          =                                              # project
OLIBS          =                                              # oracle
XLIBS          =                                              # X
SLIBS          = -lc -lm                                      # standard

LIBS           = $(MLIBS) $(PLIBS) $(OLIBS) $(XLIBS) $(SLIBS)
LIBS          += -lbcsdk



#-----------------------------------------------------------------------------#
#  5.  Specify optimization and special libraries    ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#

# The following macros retain their values regardless of superior makefiles
MY_INCPATH     = -I. -I.. -I../include
MY_LIBPATH     = -L.

# The following macro will be overridden by superior makefiles
OPT            = -g
CFLAGS         = -g $(MY_INCPATH)

#-----------------------------------------------------------------------------#
#  6.  Describe how to build this directory          ** CUSTOMIZED SECTION ** #
#-----------------------------------------------------------------------------#

all: $(EXEC_OUTPUT)

ufors: $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS) $(LIBPATH) $(LIBS)

gps.o:	gps.c $(HEADERS)
	$(CC) -c $< $(CFLAGS) -DLINUX -fpic -Di386

clean:
	rm -f $(CLEAN)

install:	ufors
	mv -f /opt/data/bin/ufors /opt/data/bin/deleteme
	cp -p ufors /opt/data/bin/
	chmod ug+rx /opt/data/bin/ufors
	chgrp shakey /opt/data/bin/ufors
	chmod ug+s /opt/data/bin/ufors
	chmod ug+rx /opt/data/bin/ida2liss
	chmod ug+rx /opt/data/bin/ida2liss
	chmod ug+s /opt/data/bin/ida2liss
	chgrp shakey /opt/data/bin/netseed
	chgrp shakey /opt/data/bin/netseed
	chmod ug+s /opt/data/bin/netseed

#=============================================================================#
#                   Do not change anything after this line                    #
#=============================================================================#

#include $(PROJECT_MACDIR)/Makefile.source.macros

#-----------------------------------------------------------------------------#
# End of source-level makefile                                                #
#-----------------------------------------------------------------------------#
