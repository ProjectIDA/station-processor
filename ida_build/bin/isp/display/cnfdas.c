#pragma ident "$Id: cnfdas.c,v 1.4 2006/02/10 02:23:42 dechavez Exp $"
/*======================================================================
 *
 * Interactively generate a new DAS configuration.
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
#include "isp_console.h"

#define MAXINPUT 80
#define MAXTOKEN 10

static void LoadNames(struct isp_dascnf *cnf, IDA *ida)
{
int i;

    for (i = 0; i < DAS_MAXSTREAM; i++) {
        if (cnf->stream[i].active) {
            idaChnlocName(ida,
                cnf->stream[i].channel,
                cnf->stream[i].filter,
                cnf->stream[i].mode,
                cnf->stream[i].name, NRTS_CNAMLEN + 1
            );
        }
    }
}

static void PrintHelp()
{
    printf("             a - ");
    printf("display active table\n");

    printf("             w - ");
    printf("display working table\n");

    printf("         again - ");
    printf("discard current edits and start over\n");

    printf("         s off - ");
    printf("deactivate stream `s' (s = 0-%d)\n", DAS_MAXSTREAM);

    printf("       s C F c - ");
    printf("define continuous stream `s' to be channel `C', filter `F'\n");

    printf("       s C F t - ");
    printf("define triggered  stream `s' to be channel `C', filter `F'\n");

    printf("  chan x y ... - ");
    printf("define detector channel(s)\n");

    printf("         key x - ");
    printf("define detector key channel to `x'\n");

    printf("         sta x - ");
    printf("set detector STA to `x'\n");

    printf("         lta x - ");
    printf("set detector LTA to `x'\n");

    printf("      thresh x - ");
    printf("set detector STA/LTA trigger level to `x'\n");

    printf("      voters x - ");
    printf("set detector voters to `x'\n");

    printf("      prebuf x - ");
    printf("set detector pre-event memory to `x' records\n");

    printf("       default - ");
    printf("make default table the working table\n");

    printf("        backup - ");
    printf("make backup table the working table\n");

    printf("go [def | bak] - ");
    printf("send working (def | bak) table to the DAS and configure\n");

    printf("          quit - ");
    printf("quit, discarding all changes to the working table\n");

}

static void ViCnf(struct isp_dascnf *cnf, IDA *ida)
{
int i, retval;
struct isp_dascnf tmpcnf;
char *name;
static char *temp = "./.cnf.tmp";
static char *command = "/usr/bin/vi ./.cnf.tmp";
static char *fid = "ViCnf";

    tmpcnf = *cnf;

    unlink(temp);
    if ((retval = isp_savcnf(temp, &tmpcnf)) != 0) {
        printf("Unable to create temp file (error %d).\n", retval);
        unlink(temp);
        return;
    }

    system(command);

    if (isp_readcnf(temp, &tmpcnf) == 0) {
        *cnf = tmpcnf;
    } else {
        printf("Problems reading file. Edits discarded.\n");
    }

    unlink(temp);

    for (i = 0; i < DAS_MAXSTREAM; i++) {
        idaChnlocName(ida,
            cnf->stream[i].channel,
            cnf->stream[i].filter,
            cnf->stream[i].mode,
            cnf->stream[i].name, NRTS_CNAMLEN + 1
        );
    }
}

int DasConfigure(struct isp_dascnf *output)
{
int i, ntoken, strm, mode, flag, itmp;
char input[MAXINPUT], *token[MAXTOKEN];
struct isp_dascnf work, active, tmpcnf;
struct isp_params params;
IDA *ida;
int quit = -1, ok = 0;
static char *fid = "Configure";

/* These should already be available */

    if (GetDasCnf(&active, 0) != 0) return quit;
    if (GetParams(&params, 0) != 0) return quit;
    ida = GetIdaHandle();

    work = active;

/* Process commands until satisfied or we give up */

    printf("Working configuration:\n");
    isp_prtdascnf(stdout, &work);

    while (1) {
        if (!Operator()) {
            printf("OPERATOR status has been lost.  ");
            printf("Reconfiguration aborted.\n");
            fflush(stdout);
            utilPause(TRUE);
            return quit;
        }
        printf("config-> "); fflush(stdout);
        fgets(input, MAXINPUT, stdin);
        ntoken = util_sparse(input, token, " ,;\n", MAXTOKEN);
        if (ntoken > 0) {
            if (strcasecmp(token[0], "a") == 0) {
                printf("Active configuration:\n");
                isp_prtdascnf(stdout, &active);
            } else if (strcasecmp(token[0], "w") == 0) {
                printf("Working configuration:\n");
                isp_prtdascnf(stdout, &work);
            } else if (strcasecmp(token[0], "again") == 0) {
                work = active;
                isp_prtdascnf(stdout, &work);
            } else if (util_isinteger(token[0])) {
                if (ntoken == 2 || ntoken == 4) {
                    strm = atoi(token[0]);
                    if (strm < 0 || strm > DAS_MAXSTREAM) {
                        printf("valid streams are between 0 and %d\n",
                            DAS_MAXSTREAM
                        );
                    } else if (ntoken == 2) {
                        if (strcasecmp(token[1], "off") == 0) {
                            work.stream[strm].active  =  0;
                            work.stream[strm].channel = -1;
                            work.stream[strm].filter  = -1;
                            work.stream[strm].name[0] =  0;
                            printf("Stream %d disabled.\n", strm);
                        } else {
                            printf("syntax error, enter ? for help\n");
                        }
                    } else if (ntoken == 4) {
                        if (strcasecmp(token[3], "t") == 0) {
                            mode = 1;
                        } else if (strcasecmp(token[3], "c") == 0) {
                            mode = 0;
                        } else {
                            mode = -1;
                        }
                        if (mode < 0) {
                            printf("command must end in `t' or `c'\n");
                        } else {
                            work.stream[strm].active  = 1;
                            work.stream[strm].channel = atoi(token[1]);
                            work.stream[strm].filter  = atoi(token[2]);
                            work.stream[strm].mode    = mode;
                            idaChnlocName(ida,
                                work.stream[i].channel,
                                work.stream[i].filter,
                                work.stream[i].mode,
                                work.stream[i].name, NRTS_CNAMLEN + 1
                            );
                            printf("Stream %d set to ", strm);
                            printf("channel %d, filter %d, ",
                                work.stream[strm].channel,
                                work.stream[strm].filter
                            );
                            if (mode) {
                                printf("triggered\n");
                            } else {
                                printf("continuous\n");
                            }
                        }
                    }
                } else {
                    printf("syntax error, enter ? for help\n");
                }
            } else if (strncasecmp(token[0], "def", 3) == 0) {
                if (isp_defcnf(&tmpcnf) != 0) {
                    printf("unable to load default configuration file: %s\n",
                        syserrmsg(errno)
                    );
                } else {
                    LoadNames(&tmpcnf, ida);
                    work = tmpcnf;
                    printf("Default configuration loaded into working table.\n");
                }
            } else if (
                strcasecmp(token[0], "backup") == 0 ||
                strcasecmp(token[0], "bak") == 0
            ) {
                if (isp_bakcnf(&tmpcnf) != 0) {
                    printf("unable to load backup configuration file: %s\n",
                        syserrmsg(errno)
                    );
                } else {
                    LoadNames(&tmpcnf, ida);
                    work = tmpcnf;
                    printf("Backup configuration loaded into working table.\n");
                }
            } else if (strcasecmp(token[0], "go") == 0) {
                flag = -1;
                if (ntoken == 1) {
                    printf("This will force the DAS to match ");
                    printf("the working configuration.\n");
                    flag = ISP_DASCNF_SET;
                } else if (ntoken == 2) {
                    if (strncasecmp(token[1], "def", 3) == 0) {
                        printf("This will load the default configuration.\n");
                        flag = ISP_DASCNF_DEF;
                    } else if (strncasecmp(token[1], "bak", 3) == 0) {
                        printf("This will load the backup configuration.\n");
                        flag = ISP_DASCNF_BAK;
                    } else if (strncasecmp(token[1], "back", 4) == 0) {
                        printf("This will load the backup configuration.\n");
                        flag = ISP_DASCNF_BAK;
                    }
                } else {
                    printf("syntax error: go [default | backup]\n");
                }
                if (flag > 0) {
                    fflush(stdout);
                    if (utilQuery("Are you ready to proceed? ")) {
                        work.flag = flag;
                        *output = work;
                        return ok;
                    }
                }

    /* Detector */

            } else if (strncasecmp(token[0], "chan", 4) == 0) {
                if (ntoken > 1) {
                    work.detect.nchan = (ntoken-1 < DAS_MAXCHAN) ?
                                         ntoken-1 : DAS_MAXCHAN;
                } else {
                    work.detect.nchan = work.detect.bitmap = 0;
                }
                work.detect.bitmap = 0;
                for (i = 0; i < work.detect.nchan; i++) {
                    itmp = atoi(token[i+1]);
                    if (itmp < 0 || itmp > DAS_MAXCHAN) {
                        printf("illegal channel: %d\n", itmp);
                        work.detect.nchan = work.detect.bitmap = 0;
                    } else {
                        work.detect.chan[i] = itmp;
                        work.detect.bitmap |= 1 << work.detect.chan[i];
                    }
                }
                if (work.detect.nchan == 0) {
                    printf("Detector disabled.\n");
                    work.detect.bitmap = 0;
                } else {
                    printf("Detector enabled, channel mask = 0x%0x",
                        work.detect.bitmap
                    );
                }

            } else if (strcasecmp(token[0], "key") == 0) {
                if (ntoken == 2) {
                    work.detect.key = atoi(token[1]);
                    printf("Key set to channel %d\n", work.detect.key);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "sta") == 0) {
                if (ntoken == 2) {
                    work.detect.sta = atoi(token[1]);
                    printf("STA set to %d\n", work.detect.sta);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "lta") == 0) {
                if (ntoken == 2) {
                    work.detect.lta = atoi(token[1]);
                    printf("LTA set to %d\n", work.detect.lta);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "thresh") == 0) {
                if (ntoken == 2) {
                    work.detect.thresh = (int) (10000.0 / atof(token[1]));
                    printf("STA/LTA trigger level set to %.2f\n", 
                        10000.0 / (float) work.detect.thresh
                    );
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "voters") == 0) {
                if (ntoken == 2) {
                    work.detect.voters = atoi(token[1]);
                    printf("Voters set to %d\n", work.detect.voters);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "maxtrig") == 0) {
                if (ntoken == 2) {
                    work.detect.maxtrig = atoi(token[1]);
                    printf("MaxTrig set to %d\n", work.detect.maxtrig);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

            } else if (strcasecmp(token[0], "prebuf") == 0) {
                if (ntoken == 2) {
                    work.detect.preevent = atoi(token[1]);
                    printf("PreBuf set to %d\n", work.detect.preevent);
                } else if (ntoken != 1) {
                    printf("syntax error\n");
                }

    /* Other */

            } else if (strcasecmp(token[0], "vi") == 0) {
                ViCnf(&work, ida);
                printf("Working configuration:\n");
                isp_prtdascnf(stdout, &work);
            } else if (strcasecmp(token[0], "q") == 0) {
                return quit;
            } else if (strcasecmp(token[0], "quit") == 0) {
                return quit;
            } else if (strcasecmp(token[0], "?") == 0) {
                PrintHelp();
            } else if (strcasecmp(token[0], "help") == 0) {
                PrintHelp();
            } else {
                printf("Unrecognized command `%s'.  Enter ? for help.\n",
                    token[0]
                );
            }
        }
    }
}

/* Revision History
 *
 * $Log: cnfdas.c,v $
 * Revision 1.4  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2003/06/11 20:55:53  dechavez
 * use new form of utilQuery and utilPause
 *
 * Revision 1.2  2002/05/14 20:46:51  dec
 * changed unmapped name from 3-char ??? to 5-char NoMap
 *
 * Revision 1.1  2000/10/19 22:24:52  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:05  dec
 * import existing IDA/NRTS sources
 *
 */
