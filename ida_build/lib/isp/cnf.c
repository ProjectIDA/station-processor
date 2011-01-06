#pragma ident "$Id: cnf.c,v 1.1.1.1 2000/02/08 20:20:27 dec Exp $"
/*======================================================================
 *
 *  Operations on ISP DAS configuration files.
 *
 *  isp_readcnf()
 *  Read an ISP/DAS configuration file.  Only limited validity checking
 *  on contents is presently supported.
 *
 *  isp_savcnf()
 *  Write an ISP/DAS configuration structure to disk in a form suitable
 *  for reading with isp_readcnf().
 *
 *  isp_setcnf()
 *  Change the active cnf file to custom, backup, or default.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <math.h>
#include "util.h"
#include "isp.h"

#define DELIMITERS  "=;,\t "
#define MAX_TOKEN   DAS_MAXCHAN
#define BUFLEN       256

int isp_readcnf(char *fname, struct isp_dascnf *cnf)
{
FILE *fp;
int i, j, status, lineno = 0, ntoken, errors;
char *token[MAX_TOKEN];
char buffer[BUFLEN];
static char *fid = "isp_readcnf";

    for (i = 0; i < DAS_MAXSTREAM; i++) cnf->stream[i].active = 0;
    cnf->detect.nchan = 0;

    if ((fp = fopen(fname, "r")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, fname, syserrmsg(errno));
        return -1;
    }

    errors = 0;
    while ((status = util_getline(fp, buffer, BUFLEN, '#', &lineno)) == 0) {

        ntoken = util_parse(buffer, token, DELIMITERS, MAX_TOKEN, 0);

        if (strcasecmp(token[0], "stream") == 0) {
            if (ntoken != 6) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            i = atoi(token[1]);
            if (i < 0 || i >= DAS_MAXSTREAM) {
                util_log(1, "%s: illegal stream code, %s line %d",
                    fid, fname, lineno
                );
                fclose(fp);
                return -2;
            }
            if (strcasecmp(token[2], "y") == 0) {
                cnf->stream[i].active = 1;
            } else if (strcasecmp(token[2], "n") == 0) {
                cnf->stream[i].active = 0;
            } else {
                util_log(1, "%s: illegal on/off flag, %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            if (cnf->stream[i].active) {
                cnf->stream[i].channel = atoi(token[3]);
                if (
                    cnf->stream[i].channel < 0 ||
                       cnf->stream[i].channel > DAS_MAXCHAN
                ) {
                    util_log(1, "%s: illegal channel, %s line %d",
                        fid, fname, lineno
                    );
                    ++errors;
                }
                cnf->stream[i].filter  = atoi(token[4]);
                if (strcasecmp(token[5], "c") == 0) {
                    cnf->stream[i].mode = 0;
                } else if (strcasecmp(token[5], "t") == 0) {
                    cnf->stream[i].mode = 1;
                } else {
                    util_log(1, "%s: illegal mode flag, %s line %d",
                        fid, fname, lineno
                    );
                    ++errors;
                }
            } else {
                cnf->stream[i].channel = -1;
                cnf->stream[i].filter  = -1;
                cnf->stream[i].mode    =  0;
            }

        } else if (strcasecmp(token[0], "Chans") == 0) {
            cnf->detect.bitmap = 0;
            cnf->detect.nchan = ntoken - 1;
            for (i = 0; i < cnf->detect.nchan; i++) {
                cnf->detect.chan[i] = atoi(token[i+1]);
                if (
                    cnf->detect.chan[i] < 0 ||
                    cnf->detect.chan[i] > DAS_MAXCHAN
                ) {
                    util_log(1, "%s: illegal detector chan, %s line %d",
                        fid, fname, lineno
                    );
                    ++errors;
                } else {
                    cnf->detect.bitmap |= 1 << cnf->detect.chan[i];
                }
            }

        } else if (strcasecmp(token[0], "Key") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.key = atoi(token[1]);

        } else if (strcasecmp(token[0], "STA") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.sta = atoi(token[1]);

        } else if (strcasecmp(token[0], "LTA") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.lta = atoi(token[1]);
        } else if (strcasecmp(token[0], "Thresh") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.thresh = (int) (10000.0 / atof(token[1]));

        } else if (strcasecmp(token[0], "Voters") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.voters = atoi(token[1]);

        } else if (strcasecmp(token[0], "MaxTrig") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.maxtrig = atol(token[1]);
        } else if (strcasecmp(token[0], "Pre-event") == 0) {
            if (ntoken != 2) {
                util_log(1, "%s: syntax error near %s line %d",
                    fid, fname, lineno
                );
                ++errors;
            }
            cnf->detect.preevent = atoi(token[1]);

        } else {
            util_log(1, "%s: syntax error near %s line %d",
                fid, fname, lineno
            );
            util_log(1, "%s: unrecognized identifer `%s'\n",
                fid, token[0]
            );
            ++errors;
        }
    }

    fclose(fp);
    cnf->flag = ISP_DASCNF_SET;
    return errors;
}

int isp_savcnf(char *fname, struct isp_dascnf *dascnf)
{
int i;
FILE *fp;
struct isp_dascnf newcnf;
static char *fid = "isp_savcnf";

    if ((fp = fopen(fname, "w")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, fname, syserrmsg(errno));
        return -1;
    }

    fprintf(fp, "# DAS configuration table\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# The \"#\" char is the comment delimiter.  All comments\n");
    fprintf(fp, "# and blank lines are ignored.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Stream definition is up to 25 lines of the form:\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Stream = Strm On Chan Filt Mode\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# where\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Strm = Stream number (0 thru 24)\n");
    fprintf(fp, "# On   = y if stream is active, n if disabled\n");
    fprintf(fp, "# Chan = DAS input channel number\n");
    fprintf(fp, "# Filt = DAS filter code\n");
    fprintf(fp, "# Mode = c for continuous, t for triggered\n");
    fprintf(fp, "#\n");
    fprintf(fp, "#        Strm   On Chan Filt Mode\n");
    fprintf(fp, "\n");

    for (i = 0; i < DAS_MAXSTREAM; i++) {
        if (dascnf->stream[i].active) {
            dascnf->stream[i].active = 1;
            dascnf->stream[i].mode = dascnf->stream[i].mode ? 1 : 0;
            fprintf(fp, "Stream = %4d   y %4d %4d %c\n",
                i, dascnf->stream[i].channel, dascnf->stream[i].filter,
                dascnf->stream[i].mode ? 't' : 'c'
            );
        } else {
            fprintf(fp, "Stream = %4d   n   -1   -1 x\n", i);
        }
    }

    fprintf(fp, "\n");
    fprintf(fp, "# Event Detector\n");
    fprintf(fp, "\n");

    fprintf(fp, "# detector channels (blank disables detector)\n");
    fprintf(fp, "Chans     =");
    for (i = 0; i < dascnf->detect.nchan; i++) {
        fprintf(fp, " %d", dascnf->detect.chan[i]);
    }
    fprintf(fp, "\n");
    fprintf(fp, "\n");
    fprintf(fp, "# detector channel whose amplitude gets reported\n");
    fprintf(fp, "Key       = %d\n", dascnf->detect.key);
    fprintf(fp, "\n");
    fprintf(fp, "# short term average, SAMPLES\n");
    fprintf(fp, "STA       = %d\n", dascnf->detect.sta);
    fprintf(fp, "\n");
    fprintf(fp, "# long  term average, SAMPLES\n");
    fprintf(fp, "LTA       = %d\n", dascnf->detect.lta);
    fprintf(fp, "\n");
    fprintf(fp, "# STA/LTA trigger ratio\n");
    fprintf(fp, "Thresh    = %.4f\n", 10000.0 / (float) dascnf->detect.thresh);
    fprintf(fp, "\n");
    fprintf(fp, "# minimum number of voters to declare a trigger\n");
    fprintf(fp, "Voters    = %d\n", dascnf->detect.voters);
    fprintf(fp, "\n");
    fprintf(fp, "# maximum trigger length, SAMPLES\n");
    fprintf(fp, "MaxTrig   = %d\n", dascnf->detect.maxtrig);
    fprintf(fp, "\n");
    fprintf(fp, "# pre-event memory, in RECORDS\n");
    fprintf(fp, "Pre-event = %d\n", dascnf->detect.preevent);

    fclose(fp);

    if (isp_readcnf(fname, &newcnf) != 0) return -2;

    for (i = 0; i < DAS_MAXSTREAM; i++) {
        if (dascnf->stream[i].active != newcnf.stream[i].active) return -3;
        if (dascnf->stream[i].active) {
            if (dascnf->stream[i].channel != newcnf.stream[i].channel) {
                return -4;
            }
            if (dascnf->stream[i].filter != newcnf.stream[i].filter) {
                return -5;
            }
            if (dascnf->stream[i].mode != newcnf.stream[i].mode) {
                return -6;
            }
        }
    }

    if (dascnf->detect.nchan != newcnf.detect.nchan) return -7;
    if (dascnf->detect.nchan == 0) return 0; /* detector disabled */

    for (i = 0; i < dascnf->detect.nchan; i++) {
        if (dascnf->detect.chan[i] != newcnf.detect.chan[i]) return -8;
    }

    if (dascnf->detect.key      != newcnf.detect.key)      return -9;
    if (dascnf->detect.sta      != newcnf.detect.sta)      return -10;
    if (dascnf->detect.lta      != newcnf.detect.lta)      return -11;
#ifdef STRICT_CHECKS
    if (dascnf->detect.thresh   != newcnf.detect.thresh)   return -12;
#endif
    if (dascnf->detect.voters   != newcnf.detect.voters)   return -13;
    if (dascnf->detect.maxtrig  != newcnf.detect.maxtrig)  return -14;
    if (dascnf->detect.preevent != newcnf.detect.preevent) return -15;

    return 0;
}

int isp_setcnf(struct isp_dascnf *dascnf)
{
static char *cnf = ISP_CNF_FILE;
static char *tmp = ISP_TMPCNF_FILE;
static char *def = ISP_DEFCNF_FILE;
static char *bak = ISP_BAKCNF_FILE;
static char *fid = "isp_setcnf";

/* Restore backup configuration */

    if (dascnf->flag == ISP_DASCNF_BAK) {
        util_log(1, "restoring backup DAS configuration");
        if (isp_readcnf(bak, dascnf) != 0) {
            util_log(1, "%s: can't load backup DAS configuration", fid);
            return -1;
        }
        unlink(cnf);
        if (rename(bak, cnf) != 0) {
            util_log(1, "%s: rename(%s, %s) failed: %s",
                fid, bak, cnf, syserrmsg(errno)
            );
            return -2;
        }

/* Load default configuration */

    } else if (dascnf->flag == ISP_DASCNF_DEF) {
        util_log(1, "restoring default DAS configuration");
        if (isp_readcnf(def, dascnf) != 0) {
            util_log(1, "%s: can't load default DAS configuration", fid);
            return -3;
        }
        unlink(bak);
        if (rename(cnf, bak) != 0) {
            util_log(1, "%s: WARNING: unable to backup cnf: %s",
                fid, syserrmsg(errno)
            );
        }
        unlink(cnf);
        if (util_cat(def, cnf, (char *) NULL) != 0) {
            util_log(1, "%s: can't copy default DAS configuration", fid);
            if (rename(cnf, bak) == 0 && isp_getcnf(dascnf) == 0) {
                util_log(1, "%s: backup configuration restored", fid);
                return -4;
            } else {
                util_log(1, "%s: can't restore backup configuration", fid);
                return -5;
            }
        }

/* Load a new configuration */

    } else if (dascnf->flag == ISP_DASCNF_SET) {
        util_log(1, "saving new DAS configuration");
        if (isp_savcnf(tmp, dascnf) != 0) {
            util_log(1, "%s: unable to save new DAS configuration", fid);
            return -5;
        }
        unlink(bak);
        if (rename(cnf, bak) != 0) {
            util_log(1, "%s: WARNING: unable to backup cnf: %s",
                fid, syserrmsg(errno)
            );
        }
        unlink(cnf);
        if (rename(tmp, cnf) != 0) {
            util_log(1, "%s: can't save new DAS configuration", fid);
            if (rename(cnf, bak) == 0 && isp_getcnf(dascnf) == 0) {
                util_log(1, "%s: backup configuration restored", fid);
                return -6;
            } else {
                util_log(1, "%s: can't restore backup configuration", fid);
                return -7;
            }
        }

/* Ignore anything else */

    } else {
        util_log(1, "%s: unrecognized flag `0x%x' ignored",
            fid, dascnf->flag
        );
        return 0;
    }

/* Now, read active configuration to make sure everything is OK */

    return isp_getcnf(dascnf);

}

/* Revision History
 *
 * $Log: cnf.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
