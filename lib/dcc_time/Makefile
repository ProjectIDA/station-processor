# DCC makefile
#
#

DCC_TIME=../dcctimelib.a
OBJS =  st_addtodelta.o \
        st_addtotime.o  \
        st_cleandate.o  \
        st_deltacomp.o  \
        st_deltaprint.o \
        st_deltatoms.o  \
        st_difftimes.o  \
        st_flttotime.o  \
        st_formdelta.o  \
        st_formatdate.o \
        st_getcurrent.o \
        st_getlocal.o   \
        st_getjulian.o  \
        st_julian.o     \
        st_minusdelta.o \
        st_parsetime.o  \
        st_printcal.o   \
        st_printdate.o  \
        st_printdec.o   \
        st_setupdate.o  \
        st_spanprint.o  \
        st_timecomp.o   \
        st_timeminusd.o \
        st_timenorm.o   \
        st_timepar.o    \
        st_timeplusd.o  \
        st_timespan.o   \
        st_timetoflt.o  \
        st_limits.o     \
        st_oracle.o     \
        st_minmax.o     \
        st_unixtimes.o

.SECONDARY:     $(OBJS)

all:    $(DCC_TIME)


$(DCC_TIME): $(DCC_TIME)($(OBJS))
	$(AR) rv $@ $(OBJS)

$(DCC_TIME)(%.o): %.o
	@true

#test:  test.o
#       $(CC) -o test test.o $(DCC_LIBS) $(ORACLE)
