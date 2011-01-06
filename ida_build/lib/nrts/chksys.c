#pragma ident "$Id: chksys.c,v 1.5 2006/02/09 19:48:01 dechavez Exp $"
/*======================================================================
 *
 * nrts_chksys: examine an nrts_sys structure for errors.
 * nrts_fixsys: repair structure corrupted by abnormal wrtdl termination
 *              DOES NOT LOCK... caller is assumed to have done so!
 *
 *====================================================================*/
#include "nrts.h"

int nrts_chksys(struct nrts_sys *sys)
{
int i, j;
off_t hoff, doff, diff;
struct nrts_asl *asl;
struct nrts_ida *ida;
struct nrts_sta *sta;
struct nrts_chn *chn;
static char *fid = "nrts_chksys";

    nrts_prtsys(NULL, sys, fid, NRTS_PRTALL);

    if (sys->info == (void *) NULL) {
        util_log(1, "%s: unsupported system type = %d", fid, sys->type);
        return -100;
    }

    switch (sys->type) {
      case NRTS_IDA:
        ida = (struct nrts_ida *) sys->info;
        break;
      case NRTS_ASL:
        asl = (struct nrts_asl *) sys->info;
        break;
      case NRTS_IDA10:
        break;
      default:
        util_log(1, "%s: unsupported system type = %d", fid, sys->type);
        return -103;
    }

    switch (sys->status) {
      case NRTS_IDLE:
        break;
      case NRTS_BUSY1:
        break;
      case NRTS_BUSY2:
        break;
      case NRTS_BUSY3:
        break;
      default:
        return -112;
    }

    if (sys->nsta < 1 || sys->nsta > NRTS_MAXSTA) {
        util_log(1, "%s: illegal nsta = %d", fid, sys->nsta);
        return -113;
    }

    hoff = doff = 0;
    for (i = 0; i < sys->nsta; i++) {
        sta = sys->sta + i;
        if (sta->nchn < 1 || sta->nchn > NRTS_MAXCHN) {
            util_log(1, "%s: illegal sta[%d].nchan = %d", fid, i, sta->nchn);
            return -114;
        }
        for (j = 0; j < sta->nchn; j++) {
            chn = sta->chn + j;

            if (chn->hdr.nrec <= 0) {
                util_log(1, "%s: illegal sta[%d].chn[%d].hdr.nrec = %d",
                         fid, i, j, chn->hdr.nrec);
                return -115;
            }

            if (chn->hdr.nrec != chn->dat.nrec) {
                util_log(1, "%s: sta[%d].chn[%d] hdr.nrec != dat.nrec",fid,i,j);
                return -116;
            }

            if (chn->hdr.hide >= chn->hdr.nrec || chn->hdr.hide < 1) {
                util_log(1, "%s: illegal sta[%d].chn[%d].hdr.hide = %d",
                         fid, i, j, chn->hdr.hide);
                return -117;
            }

            if (chn->hdr.hide != chn->dat.hide) {
                util_log(1, "%s: sta[%d].chn[%d] hdr.hide != dat.hide",fid,i,j);
                return -118;
            }

            if (chn->hdr.siz != (chn->hdr.len * chn->hdr.nrec)) {
                util_log(1, "%s: sta[%d].chn[%d] hdr.siz != len*nrec",fid,i,j);
                return -119;
            }

            if (chn->hdr.off != hoff) {
                util_log(1, "%s: sta[%d].chn[%d] hdr.off != %ld",fid,i,j,hoff);
                return -120;
            }

            hoff += chn->hdr.siz;

            if (chn->dat.siz != (chn->dat.len * chn->dat.nrec)) {
                util_log(1, "%s: sta[%d].chn[%d] dat.siz != len*nrec",fid,i,j);
                return -121;
            }

            if (chn->dat.off != doff) {
                util_log(1, "%s: sta[%d].chn[%d] dat.off != %ld",fid,i,j,doff);
                return -122;
            }

            doff += chn->dat.siz;

            switch (chn->status) {
              case NRTS_IDLE:
                if (chn->hdr.oldest < 0) {
                    util_log(1, "%s: illegal sta[%d].chn[%d] hdr.oldest",
                        fid, i, j, chn->hdr.oldest
                    );
                    return -123;
                } else if (chn->hdr.yngest < 0) {
                    if (chn->hdr.yngest != -1 || chn->hdr.oldest != 0) {
                        util_log(1, "%s: illegal sta[%d].chn[%d] hdr.yngest",
                            fid, i, j, chn->hdr.oldest
                        );
                        return -124;
                    }
                } else if ((diff = chn->hdr.oldest - chn->hdr.yngest) == 0) {
                    if (chn->hdr.oldest != 0) {
                        util_log(1, "%s: error -125: %ld %ld %ld",
                            fid, chn->hdr.oldest, chn->hdr.yngest, diff
                        );
                        return -125;
                    }
                } else if (diff < 0) {
                    if ((diff = -diff) > chn->hdr.nrec - chn->hdr.hide) {
                        return -126;
                    }
                } else {
                    if (diff != chn->hdr.hide + 1) {
                        util_log(1, "%s: error -127: (%d,%d) %ld %ld %ld %ld",
                            fid, i, j, chn->hdr.oldest, chn->hdr.yngest, diff,
                            chn->hdr.hide
                        );
                        return -127;
                    }
                }
                break;
              case NRTS_BUSY1:
                break;
              case NRTS_BUSY2:
                break;
              case NRTS_BUSY3:
                break;
              default:
                return -128;
            }
        }
    }

    return 0;
}

int nrts_fixsys(struct nrts_sys *sys)
{
int i, j, count = 0;
long oldest, yngest, lend, diff;
struct nrts_chn *chn;
static char *fid = "nrts_fixsys";

/*  If pid field is filled, test to see that the process is not active  */

    if (sys->pid > 0 && sys->pid != getpid()) {
#ifndef _WIN32
        if (kill(sys->pid, SIGHUP) == 0) {
            util_log(9, "%s: process %d is still alive", fid, sys->pid);
        } else if (errno == ESRCH) {
            util_log(9, "%s: process %d is inactive", fid, sys->pid);
        } else {
            util_log(1, "%s: kill(%d, SIGHUP): %s", fid, sys->pid, syserrmsg(errno));
        }
#else
/* WIN32 code will be here */
#endif

           return -1;
    }

/*  Look for and correct channels which were left in a busy state  */

    count = 0;
    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            chn = sys->sta[i].chn + j;
            if (chn->status == NRTS_BUSY1) {
                ++count;
                util_log(1, "sta[%d].chn[%d] is BUSY1", i, j);
                util_log(1, "oldest = %ld yngest = %ld",
                    chn->hdr.oldest, chn->hdr.yngest
                );
                if (chn->hdr.oldest >= 0  && chn->hdr.yngest >= 0) {
                    oldest = chn->hdr.oldest - 1;
                    if (oldest < 0) oldest = chn->hdr.nrec - 1;
                    util_log(1, "reset (%d, %d) oldest to %ld", i, j, oldest);
                    yngest = chn->hdr.yngest - 1;
                    if (yngest < 0) yngest = chn->hdr.nrec - 1;
                    util_log(1, "reset (%d, %d) yngest to %ld", i, j, yngest);
                    if (oldest < yngest) {
                        diff = yngest - oldest;
                        if (diff < chn->hdr.nrec - chn->hdr.hide) {
                            lend = chn->hdr.nrec - chn->hdr.hide;
                        } else {
                            lend = yngest + 1;
                            if (lend == chn->hdr.nrec) lend = 0;
                        }
                    } else {
                        lend = yngest + 1;
                    }
                    util_log(1, "reset (%d, %d) lend to %ld", i, j, lend);
                    chn->hdr.backup.oldest = oldest;
                    chn->hdr.backup.yngest = yngest;
                    chn->hdr.backup.lend   = lend;
                    chn->status = NRTS_BUSY3;
                    chn->hdr.oldest = oldest;
                    chn->hdr.yngest = yngest;
                    chn->hdr.lend   = lend;
                }
                chn->status = NRTS_IDLE;
            } else if (chn->status == NRTS_BUSY2) {
                ++count;
                util_log(1, "sta[%d].chn[%d] is BUSY2", i, j);
                if (chn->hdr.oldest >= 0  && chn->hdr.yngest >= 0) {
                    chn->hdr.oldest = chn->hdr.backup.oldest;
                    chn->hdr.yngest = chn->hdr.backup.yngest;
                    chn->hdr.lend   = chn->hdr.backup.lend;
                }
                chn->status = NRTS_IDLE;
            } else if (chn->status == NRTS_BUSY3) {
                ++count;
                util_log(1, "sta[%d].chn[%d] is BUSY3", i, j);
                chn->hdr.oldest = chn->hdr.backup.oldest;
                chn->status = NRTS_IDLE;
            } else if (chn->status == NRTS_IDLE) {
                if (chn->hdr.oldest < 0) {
                    ;
                } else if (chn->hdr.yngest < 0) {
                    ;
                } else if ((diff = chn->hdr.oldest - chn->hdr.yngest) == 0) {
                    ;
                } else if (diff < 0) {
                    ;
                } else if (diff != chn->hdr.hide + 1) {
                    util_log(1, "illegal (%d, %d) %ld %ld %ld", i, j,
                        chn->hdr.oldest, chn->hdr.yngest,
                        chn->hdr.lend, chn->hdr.hide
                    );
                    oldest = chn->hdr.oldest - 1;
                    if (oldest < 0) oldest = chn->hdr.nrec - 1;
                    util_log(1, "reset (%d, %d) oldest to %ld", i, j, oldest);
                    chn->hdr.backup.yngest = yngest;
                    yngest = chn->hdr.oldest - (chn->hdr.hide + 1);
                    util_log(1, "reset (%d, %d) yngest to %ld", i, j, yngest);
                    lend = yngest + 1;
                    chn->hdr.backup.oldest = oldest;
                    chn->hdr.backup.yngest = yngest;
                    chn->hdr.backup.lend   = lend;
                    chn->status = NRTS_BUSY3;
                    chn->hdr.oldest = oldest;
                    chn->hdr.yngest = yngest;
                    chn->hdr.lend   = lend;
                    chn->status = NRTS_IDLE;
                }
            }
        }
    }

    if (count == 0) util_log(1, "all channels are IDLE");

    if (sys->pid) {
        util_log(1, "clearing sys->pid (was %d)", sys->pid);
        sys->pid = 0;
    }

/*  Should now pass the checker  */

    return nrts_chksys(sys);
}

/* Revision History
 *
 * $Log: chksys.c,v $
 * Revision 1.5  2006/02/09 19:48:01  dechavez
 * removed unused IDADB argument
 *
 * Revision 1.4  2005/05/06 22:17:45  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.3  2004/09/28 22:42:46  dechavez
 * fixed ancient (10+ years) bug in nrts_fixsys in setting dl status flag
 * after repair
 *
 * Revision 1.2  2004/06/24 17:39:52  dechavez
 * removed unnecessary includes, WIN32 "port" (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
