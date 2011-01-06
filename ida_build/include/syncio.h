#pragma ident "$Id: syncio.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"
#define syncio_h_included

#include <sys/syncio.h>

struct syncio_status {
    struct syncStat   adap;
    struct syncStatus port;
};

static struct syncCtl null_sync_cb = {
    SYNC_SIGNATURE, /* .signature */
    SYNC_VERSION,   /* .version   */
    0,              /* .result    */
    0,              /* .length    */
    {               /* .u         */
        0,
    }
};

static struct syncCfg syncio_def_cfg = {
    9216000,       /* adapter CPU clock rate, Hz */
    SYNCFG_V35,    /* interface type             */
    SYNCFG_FM0,    /* data encoding              */
    SYNCFG_INTCLK, /* clock source               */
    57600,         /* baud rate                  */
    1234,          /* maximum transmit unit      */
    0,             /* options                    */
    0              /* reserved                   */
};

#ifdef __STDC__

int syncio_init(
    char *,
    struct syncCfg *
);

struct syncio_status *syncio_getstat(
    int
);

void syncio_prtstat(
    FILE *,
    struct syncio_status *
);

#endif
#endif

/* Revision History
 *
 * $Log: syncio.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
