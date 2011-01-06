#pragma ident "$Id: cdio.c,v 1.7 2005/10/11 18:29:52 dechavez Exp $"
/*======================================================================
 *
 *  Utilities to facilitate generation of CD rom archives
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_CDIO

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern char *Syscode;

static MUTEX LocalMutex = MUTEX_INITIALIZER;
static BOOL ActiveFlag = FALSE;
static BOOL Enabled    = FALSE;
static BOOL ForceFlag  = FALSE;

#define FORCE_ISO_SCRIPT "/usr/nrts/scripts/isiStageTeeFiles"
#define STATS_SCRIPT     "/usr/nrts/scripts/isiGetIsoStats"

static char CdStatsFile[MAXPATHLEN+1];
static char CommandLine[MAXPATHLEN+128];

static BOOL ExecuteSystemCommand(char *CommandLine, BOOL verbose)
{
int status;

    if (verbose) util_log(1, "%s", CommandLine);
    status = system(CommandLine);
    if (status == -1 || status == 256) {
        util_log(1, "system(%s) failed: %s",
            CommandLine, strerror(errno)
        );
        return FALSE;
    }
    return TRUE;
}

void UpdateCdStats(BOOL verbose)
{
FILE *fp;
int files, bytes, images, capacity;
static char *fid = "UpdateCdStats";

    sprintf(CommandLine, "%s %s %s", STATS_SCRIPT, Syscode, CdStatsFile);

    if (!ExecuteSystemCommand(CommandLine, verbose)) return;

    if ((fp = fopen(CdStatsFile, "r")) == NULL) {
        util_log(1, "%s: fopen: %s: %s",
            fid, CdStatsFile, strerror(errno)
        );
        return;
    }

    fscanf(fp, "%d %d %d %d", &files, &bytes, &images, &capacity);
    fclose(fp);

    MUTEX_LOCK(&Status->lock);
        Status->output.file = files;
        Status->buffer.nrec = bytes;
        Status->output.nrec = images;
        Status->buffer.capacity = capacity;
    MUTEX_UNLOCK(&Status->lock);

    unlink(CdStatsFile);
}

int cdio_init()
{
THREAD tid;
static char *fid = "cdio_init";

    sprintf(CdStatsFile, "/var/tmp/ispdCdStats.%d", getpid());
    MUTEX_LOCK(&Status->lock);
        memset(Status->output.device, 0, ISP_DEVNAMELEN);
        Status->output.type  = ISP_OUTPUT_CDROM;
        Status->output.state = ISP_OUTPUT_UNKNOWN;
    MUTEX_UNLOCK(&Status->lock);

    MUTEX_LOCK(&LocalMutex);
        Enabled = TRUE;
    MUTEX_UNLOCK(&LocalMutex);

    return 0;
}

int eject_cdrom(int unused)
{
static char *CommandLine = "eject cdrom";

    return ExecuteSystemCommand(CommandLine, TRUE) ? 0 : -1;
}

static THREAD_FUNC FlushIsoImageThread(void *unused)
{
static char *fid = "FlushIsoImageThread";

    sprintf(CommandLine, "%s FORCE %s", FORCE_ISO_SCRIPT, Syscode);
    ExecuteSystemCommand(CommandLine, TRUE);
    UpdateCdStats(FALSE);
    ActiveFlag = FALSE;
}

void FlushIsoImage()
{
THREAD tid;
static char *fid = "FlushIsoImage";

    MUTEX_LOCK(&LocalMutex);
        if (Enabled && !ActiveFlag) {
            ActiveFlag = TRUE;
            if (!THREAD_CREATE(&tid, FlushIsoImageThread, NULL)) {
                util_log(1, "%s: failed to start FlushIsoImageThread", fid);
                ispd_die(MY_MOD_ID + 2);
            }
        }
    MUTEX_UNLOCK(&LocalMutex);
}

/* Revision History
 *
 * $Log: cdio.c,v $
 * Revision 1.7  2005/10/11 18:29:52  dechavez
 * eliminated UpdateCdStatsThread (not needed since the stats now get updated
 * whenever a client connects), support for improved stage and stat scripts
 *
 * Revision 1.6  2005/08/26 20:18:00  dechavez
 * moved to ISI related scripts
 *
 * Revision 1.5  2003/10/02 20:10:58  dechavez
 * eject cdrom instead of /dev/cdrom
 *
 * Revision 1.4  2002/03/15 22:51:36  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.3  2002/02/27 00:28:56  dec
 * move all system() stuff to ExecuteSystemCommand for debugging purposes
 *
 * Revision 1.2  2002/02/22 23:59:01  dec
 * Fixed error with force flag causing spurious premature ISO volume
 * generation, explicitly update status following zips
 *
 * Revision 1.1  2001/12/20 18:27:43  dec
 * created
 *
 */
