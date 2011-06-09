#ifndef __LIB330_H__
#define __LIB330_H__

#ifndef libarchive_h
#include <libarchive.h>
#endif
#ifndef libclient_h
#include <libclient.h>
#endif
#ifndef libcmds_h
#include <libcmds.h>
#endif
#ifndef libcompress_H
#include <libcompress.h>
#endif
#ifndef libcont_h
#include <libcont.h>
#endif
#ifndef libctrldet_h
#include <libctrldet.h>
#endif
#ifndef libacvrt_h
#include <libcvrt.h>
#endif
#ifndef libdetect_h
#include <libdetect.h>
#endif
#ifndef libdss_h
#include <libdss.h>
#endif
#ifndef libfilters_h
#include <libfilters.h>
#endif
#ifndef liblogs_h
#include <liblogs.h>
#endif
#ifndef libmd5_h
#include <libmd5.h>
#endif
#ifndef libmsgs_h
#include <libmsgs.h>
#endif
#ifndef libnetserv_h
#include <libnetserv.h>
#endif
#ifndef libopaque_h
#include <libopaque.h>
#endif
#ifndef libpoc_h
#include <libpoc.h>
#endif
#ifndef libsampcfg_h
#include <libsampcfg.h>
#endif
#ifndef libsampglob_h
#include <libsampglob.h>
#endif
#ifndef libsample_h
#include <libsample.h>
#endif
#ifndef libseed_h
#include <libseed.h>
#endif
#ifndef libslider_h
#include <libslider.h>
#endif
#ifndef libstats_h
#include <libstats.h>
#endif
#ifndef libstrucs_h
#include <libstrucs.h>
#endif
#ifndef libsupport_h
#include <libsupport.h>
#endif
#ifndef libtokens_h
#include <libtokens.h>
#endif
#ifndef libtypes_h
#include <libtypes.h>
#endif
#ifndef libverbose_h
#include <libverbose.h>
#endif
#ifndef platform_h
#include <platform.h>
#endif
#ifndef q330cvrt_h
#include <q330cvrt.h>
#endif
#ifndef q330io_h
#include <q330io.h>
#endif
#ifndef q330types_h
#include <q330types.h>
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

typedef uint8_t bool;

typedef struct mholdqtype {
    struct mholdqtype *link;
    string m;
} mholdqtype;
typedef mholdqtype *tmhqp;

#endif

