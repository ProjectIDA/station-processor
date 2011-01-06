#pragma ident "$Id: config.c,v 1.9 2006/10/01 12:52:21 dechavez Exp $"
/*======================================================================
 *
 *  DAS configuration reconciliation procedure.
 *
 *  It works by rebooting the DAS to get a reliable IDDATA record,
 *  comparing it to the ISP's version of the configuration, and
 *  sending modification commands for those parts of the configuration
 *  that differ.
 *
 *  The process is repeated indefinitely until the two match.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <assert.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_CONFIG

#define DAS_OUTMASK 0x01

extern ISP_PARAMS *Params;
extern struct isp_dascnf *DasCnf;

static char *compare_stream(struct isp_dascnf *cnf, int index)
{
short stmp, output = DAS_OUTMASK, gain = -1;
char *ptr;
static char cmd[14];

    assert(sizeof(short) == 2);

    if (
       (!DasCnf->stream[index].active && !cnf->stream[index].active)||
        DasCnf->stream[index].active  == cnf->stream[index].active  &&
        DasCnf->stream[index].channel == cnf->stream[index].channel &&
        DasCnf->stream[index].filter  == cnf->stream[index].filter  &&
        DasCnf->stream[index].mode    == cnf->stream[index].mode
    ) return (char *) NULL;

    cmd[0] = sizeof(cmd) - 1;
    cmd[1] = DAS_RECONFIGURE_CMD;
    cmd[cmd[0]] = DAS_CONFIRM_CMD;

    ptr  = cmd + 2;
    *ptr = (char) index; ++ptr;

    stmp = cnf->stream[index].active ? cnf->stream[index].channel : -1;
    SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->stream[index].filter; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->stream[index].mode; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = output; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = gain; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    return cmd;
}

static char *compare_detect(struct isp_dascnf *cnf)
{
short stmp;
long ltmp;
char *ptr;
static char cmd[21];

    assert(sizeof(short) == 2);
    assert(sizeof(long)  == 4);

    ltmp = labs((long) DasCnf->detect.thresh - (long) cnf->detect.thresh);
    if (ltmp != 0) util_log(1, "compare_detect: ltmp = %d (%d - %d)", ltmp, (long) DasCnf->detect.thresh, (long) cnf->detect.thresh);

    if (
        ltmp < 5 &&
        DasCnf->detect.bitmap   == cnf->detect.bitmap  &&
        DasCnf->detect.key      == cnf->detect.key     &&
        DasCnf->detect.sta      == cnf->detect.sta     &&
        DasCnf->detect.lta      == cnf->detect.lta     &&
        DasCnf->detect.maxtrig  == cnf->detect.maxtrig &&
        DasCnf->detect.voters   == cnf->detect.voters  &&
        DasCnf->detect.preevent == cnf->detect.preevent
    ) return (char *) NULL;

    cmd[0] = sizeof(cmd) - 1;
    cmd[1] = DAS_RECONFIGURE_DETECTOR_CMD;
    cmd[cmd[0]] = DAS_CONFIRM_CMD;

    ptr = cmd + 2;

    stmp = cnf->detect.bitmap; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->detect.key; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->detect.sta; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->detect.lta; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = ((float) cnf->detect.thresh / 10000.0) * 32768; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    ltmp = cnf->detect.maxtrig; LSWAB(&ltmp, 1);
    memcpy(ptr, &ltmp, 4); ptr += 4;

    stmp = cnf->detect.voters; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    stmp = cnf->detect.preevent; SSWAB(&stmp, 1);
    memcpy(ptr, &stmp, 2); ptr += 2;

    return cmd;
}

void SaveDasConfig(IDA_CONFIG_REC *config)
{
int i, index;
short tst;

/* Initialize the global configuration table */

    for (i = 0; i < ISP_NSTREAM; i++) DasCnf->stream[i].active = 0;

    for (i = 0; i < config->nstream; i++) {
        if (config->table[i].dl_channel >= 0) {
            if ((index = config->table[i].dl_stream) >= ISP_NSTREAM) {
                util_log(1, "BOGUS DAS STREAM CODE `%d', ignored",
                    config->table[i].dl_stream
                );
                config->table[i].dl_channel = -1;
            }
        }
    }

/* Copy over stream map */

    for (i = 0; i < config->nstream; i++) {
        if (config->table[i].dl_channel >= 0) {
            index = config->table[i].dl_stream;
            DasCnf->stream[index].active  = 1;
            DasCnf->stream[index].channel = config->table[i].dl_channel;
            DasCnf->stream[index].filter  = config->table[i].dl_filter;
            DasCnf->stream[index].mode    = config->table[i].mode;
            idaChnlocName(
                Params->ida,
                DasCnf->stream[index].channel,
                DasCnf->stream[index].filter,
                DasCnf->stream[index].mode,
                DasCnf->stream[index].name,
                NRTS_CNAMLEN
            );
        }
    }

/* Copy over detector parameters */

    DasCnf->detect.bitmap   = (int) config->detect.chan;
    DasCnf->detect.key      = (int) config->detect.key;
    DasCnf->detect.sta      = (int) config->detect.sta;
    DasCnf->detect.lta      = (int) config->detect.lta;
    DasCnf->detect.thresh   = (int) config->detect.thresh;
    DasCnf->detect.maxtrig  = (long) config->detect.maxlen;
    DasCnf->detect.voters   = (int) config->detect.voters;
    DasCnf->detect.preevent = (int) config->detect.memory;

/* Pull the channels out of the bitmap */

    DasCnf->detect.nchan = 0;
    for (i = 0; i < sizeof(DasCnf->detect.bitmap) * 8; i++) {
        if ((DasCnf->detect.bitmap >> i) & 0x01) {
            DasCnf->detect.chan[DasCnf->detect.nchan++] = i;
        }
    }

    set_have_dascnf();
}

static void verify_config()
{
struct {
    int count;
    int done;
} stream, detect;
int i, count, first;
char *cmd;
struct isp_dascnf cnf;
static char *fid = "verify_config";

    util_log(2, "Begin DAS configuration verfication");
    MUTEX_LOCK(&DasCnf->mutex);

    if (isp_getcnf(&cnf) != 0) {
        util_log(1, "%s: FATAL ERROR: can't read configuration file", fid);
        ispd_die(1);
    }

    first = 1;
    stream.done = detect.done = 0;
    while (!stream.done || !detect.done) {

        if (!first) util_log(2, "configuration incomplete... try again");


    /* Get the current DAS configuration */

        count = 0;
        while (!have_dascnf()) {
            if (count % Params->iddatato == 0) {
                if (count) util_log(1, "no IDDATA from DAS");
                das_reboot();
            }
            sleep(1);
            ++count;
        }

    /* Verify stream map */

        stream.count = 0;
        for (i = 0; i < ISP_NSTREAM; i++) {

            if ((cmd = compare_stream(&cnf, i)) != (char *) NULL) {

                if (output_enabled()) disable_output(1);
                if (stream.count == 0) {
                    util_log(1, "DAS stream map does not match ISP");
                }
                util_log(1, "DAS has:  %2d = %2d %2d %2d", i,
                    DasCnf->stream[i].active ? DasCnf->stream[i].channel : -1,
                    DasCnf->stream[i].filter, DasCnf->stream[i].mode
                );
                util_log(1, "reset to: %2d = %2d %2d %2d", i,
                    cnf.stream[i].active ? cnf.stream[i].channel : -1,
                    cnf.stream[i].filter, cnf.stream[i].mode
                );

                das_command(cmd, 1); sleep(1);
                ++stream.count;
            }
        }

        if (stream.count == 0) {
            util_log(1, "DAS stream map verified");
            stream.done = 1;
        }

    /* Verify detector parameters */

        detect.count = 0;
        if ((cmd = compare_detect(&cnf)) != (char *) NULL) {
            util_log(1, "WARNING: DAS detector does not match ISP");
            if (output_enabled()) disable_output(1);
            util_log(1, "DAS has:     0x%0x, %d, %d, %d, %d, %d, %d, %d",
                DasCnf->detect.bitmap, DasCnf->detect.key,
                DasCnf->detect.sta, DasCnf->detect.lta,
                DasCnf->detect.thresh,
                DasCnf->detect.voters, DasCnf->detect.maxtrig,
                DasCnf->detect.preevent
            );
            util_log(1, "ISP expects: 0x%0x, %d, %d, %d, %d, %d, %d, %d",
                cnf.detect.bitmap, cnf.detect.key, cnf.detect.sta, 
                cnf.detect.lta, cnf.detect.thresh,
                cnf.detect.voters, cnf.detect.maxtrig,
                cnf.detect.preevent
            );

//            das_command(cmd, 1); sleep(1);
//            ++detect.count;
            detect.done = 1;

        } else {
            util_log(1, "DAS detector verified");
            detect.done = 1;
        }

        if ((count = stream.count + detect.count) != 0) {
            util_log(2, "%s: %d configuration commands sent", fid, count);
            clear_have_dascnf();
            sleep(1);
        }
        first = 0;
    }

    set_config(ISP_CONFIG_VERIFIED);
    enable_output(1);
    set_have_dascnf();
    DasCnf->flag = ISP_DASCNF_SET;
    set_cnflag();
    MUTEX_UNLOCK(&DasCnf->mutex);

    return;

}

static THREAD_FUNC DasConfigThread(void *dummy)
{
    verify_config();
    THREAD_EXIT(0);
}

void ConfigureDas(int wait)
{
THREAD tid;
int already;
static MUTEX mp = MUTEX_INITIALIZER;
static char *fid = "ConfigureDas";

    MUTEX_LOCK(&mp);
        if (config_status() == ISP_CONFIG_INPROGRESS) {
            already = 1;
        } else {
            set_config(ISP_CONFIG_INPROGRESS);
            already = 0;
        }
    MUTEX_UNLOCK(&mp);

    if (already) return;

    if (!have_dascnf()) {
        util_log(1, "UNABLE TO UPDATE DAS CONFIG: NO CURRENT DAS CONFIG");
        return;
    }

/* If wait flag is set, do the configuration here */

    if (wait) {
        verify_config();
        return;
    }

/* Otherwise, leave a thread behind to take care of it */

    if (!THREAD_CREATE(&tid, DasConfigThread, NULL)) {
        util_log(1, "%s: failed to create DasConfigThread", fid);
        ispd_die(MY_MOD_ID + 1);
    }

    return;
}

/* Revision History
 *
 * $Log: config.c,v $
 * Revision 1.9  2006/10/01 12:52:21  dechavez
 * Don't attempt to reconcile DAS detector.  Just log a warning message.
 *
 * Revision 1.8  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.7  2005/07/26 00:58:14  dechavez
 * initial ISI dl support (2.4.6d)
 *
 * Revision 1.6  2003/12/10 06:25:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.5  2002/05/14 20:47:29  dec
 * use NRTS_CNAMLEN instead of hard-coded 3 in stream name
 *
 * Revision 1.4  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.3  2001/05/20 17:43:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
