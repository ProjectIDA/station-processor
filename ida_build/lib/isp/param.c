#pragma ident "$Id"
/*======================================================================
 *
 *  Load ISPD run-time configuration parameters from disk.
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
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"
#include "isp.h"
#include "iboot.h"

#define DELIMITERS  "= "
#define NUM_TOKEN      2
#define MAX_TOKEN     16
#define BUFLEN       ISP_BUFLEN

/* Default values */

#define DEFAULT_ISP_STA          ((char *) NULL)
#define DEFAULT_ISP_HOME         ((char *) NULL)
#define DEFAULT_ISP_OPER         "ispmgr"
#define DEFAULT_ISP_PKTLEN       1024
#define DEFAULT_ISP_INPUT        "/dev/das"
#define DEFAULT_ISP_IBAUD        ISP_MAX_BAUD
#define DEFAULT_ISP_OBAUD        ISP_MAX_BAUD
#define DEFAULT_ISP_NODATATO     60
#define DEFAULT_ISP_IDDATATO     80
#define DEFAULT_ISP_RAWQ         64  /* Raw data packets for processing     */
#define DEFAULT_ISP_BARQ         16  /* Raw time stamped barometer samples  */
#define DEFAULT_ISP_DPMQ         16  /* Raw time stamped DPM samples        */
#define DEFAULT_ISP_CMDQ         16  /* Operator commands                   */
#define DEFAULT_ISP_OUTPUT       "cdrom"
#define DEFAULT_ISP_BFACT        32
#define DEFAULT_ISP_OBUF         32
#define DEFAULT_ISP_NUMOBUF      2
#define DEFAULT_ISP_TTYTO        1
#define DEFAULT_ISP_MTU          5
#define DEFAULT_ISP_DELAY        10
#define DEFAULT_ISP_DAS_PKTREV   8
#define DEFAULT_ISP_SAN_PKTREV   10
#define DEFAULT_ISP_CHANMAP      ((char *) NULL)
#define DEFAULT_ISP_STATINT      ISP_MIN_STATINT
#define DEFAULT_ISP_SAVEDASSTATS 0
#define DEFAULT_ISP_SOCKETTO     10
#define DEFAULT_ISP_MAXCLIENTS   5
#define DEFAULT_ISP_TCPPORT      ISP_DEFAULT_COMMAND_PORT
#define DEFAULT_ISP_NRTS         1
#define DEFAULT_ISP_INJECT       0
#define DEFAULT_ISP_IDA_FLAGS    0
#define DEFAULT_ISP_SAN_DATA_PORT 14002
#define DEFAULT_ISP_SAN_CMND_PORT 16000
#define DEFAULT_ISP_SAN_TIMEOUT   30
#define DEFAULT_ISP_SAN_SOHINT    5
#define DEFAULT_ISP_PARO_PR 119
#define DEFAULT_ISP_PARO_UF "68947.57"
#define DEFAULT_ISP_RT593         0
#define DEFAULT_ISP_IBOOT        {IBOOT_DEFAULT_SERVER, IBOOT_DEFAULT_PORT, IBOOT_DEFAULT_PASSWD, 180, 10}

/* Limits */

#define ISP_MIN_BAUD    1200 /* smallest supported baud rate          */
#define ISP_MAX_BAUD   38400 /* largest supported baud rate           */
#define ISP_MIN_STATINT   30 /* min DAS status request interval       */
#define ISP_MIN_SAN_TIMEOUT 30
#define ISP_MIN_SAN_SOHINT   5

/* Determine media type code from string */

int ispGetMediaType(ISP_PARAMS *params)
{
#define DEVICE_PREFIX "/dev"

    if (strncasecmp(params->odev, DEVICE_PREFIX, strlen(DEVICE_PREFIX)) == 0) {
        return ISP_OUTPUT_TAPE;
    } else if (strcasecmp(params->odev, "cdrom") == 0) {
        return ISP_OUTPUT_CDROM;
    } else {
        return ISP_OUTPUT_NONE;
    }
}

/* Verify contents */

static BOOL CheckParam(char *sta, ISP_PARAMS *params, ISP_SERVER *server, char *path)
{
FILE *fp;
BOOL retval = TRUE;
int status, media, problems = 0;
char buffer[MAXPATHLEN+16];
struct stat buf;

/* Make sure legal values have been given */

    memset(params->sta, 0, ISP_SNAMLEN);
    if (strlen(sta) > ISP_MAXSYSNAM) {
        fprintf(stderr, "%s: ", path);
        fprintf(stderr, "Station code `%s' is too long!\n", sta);
        retval = FALSE;
    } else {
        strcpy(params->sta, util_ucase(sta));
    }

    if (params->ttyto < 1) {
        fprintf(stderr, "%s: illegal ttyto = %d\n", path, params->ttyto);
        retval = FALSE;
    }
    if (params->rawq < 1) {
        fprintf(stderr, "%s: illegal rawq = %d\n", path, params->rawq);
        retval = FALSE;
    }
    if (params->cmdq < 1) {
        fprintf(stderr, "%s: illegal cmdq = %d\n", path, params->cmdq);
        retval = FALSE;
    }

    if (params->digitizer == ISP_DAS) {
        if (params->ibaud < ISP_MIN_BAUD || params->ibaud > ISP_MAX_BAUD) {
            fprintf(stderr, "%s: illegal ibaud = %d\n", path, params->ibaud);
            retval = FALSE;
        }
        if (params->obaud < ISP_MIN_BAUD || params->obaud > ISP_MAX_BAUD) {
            fprintf(stderr, "%s: illegal ibaud = %d\n", path, params->obaud);
            retval = FALSE;
        }
        if (params->mtu < 1) {
            fprintf(stderr, "%s: illegal mtu = %d\n", path, params->mtu);
            retval = FALSE;
        }
        if (params->iddatato < 1) {
            fprintf(stderr, "%s: illegal iddatato = %d\n", path, params->iddatato);
            retval = FALSE;
        }
        if (params->nodatato < 1) {
            fprintf(stderr, "%s: illegal nodatato = %d\n", path, params->nodatato);
            retval = FALSE;
        }
        if (params->ttyto > params->nodatato || params->ttyto > params->iddatato) {
            fprintf(stderr, "%s: ttyto (%d) !< nodata (%d) or iddata (%d)",
                path, params->ttyto, params->nodatato, params->iddatato
            );
            retval = FALSE;
        }
    }

    if (params->statint < ISP_MIN_STATINT) {
        fprintf(stderr, "%s: illegal statint = %d\n", path, params->statint);
        retval = FALSE;
    }

    media = ispGetMediaType(params);
    if (media == ISP_OUTPUT_TAPE) {
        if (params->bfact < 1) {
            fprintf(stderr, "%s: illegal bfact = %d\n", path, params->bfact);
            retval = FALSE;
        }
        if (params->obuf < 1) {
            fprintf(stderr, "%s: illegal obuf = %d\n", path, params->obuf);
            retval = FALSE;
        }
        if (params->numobuf < 1) {
            fprintf(stderr, "%s: illegal numobuf = %d\n", path, params->numobuf);
            retval = FALSE;
        }
    } else if (media == ISP_OUTPUT_CDROM) {
        params->bfact = 0;
        params->obuf = 1;
    }

    if (params->baro.enabled) {
        if (params->barq < 10) {
            fprintf(stderr, "%s: illegal barq = %d\n", path, params->barq);
            retval = FALSE;
        }
        if (params->baro.pr < 1 || params->baro.pr > 16383) {
            fprintf(stderr, "%s: illegal barometer PR = %d\n",
                path, params->baro.pr
            );
            retval = FALSE;
        }
        if (!params->clock.enabled) {
            fprintf(stderr, "%s: baro requires clock, but none given\n", path);
            retval = FALSE;
        } else {
            if (params->digitizer == ISP_DAS) {
                if (params->baro.sint == 1) {
                    params->baro.filt = 0;
                } else if (params->baro.sint == 2) {
                    params->baro.filt = 10;
                } else if (params->baro.sint == 10) {
                    params->baro.filt = 2;
                } else {
                    fprintf(stderr, "%s: baro sint = %d != 1 | 2 | 10\n", 
                        path , params->baro.sint
                    );
                    retval = FALSE;
                }
            }
        }
    } else {
        params->barq = 0;
    }

    if (params->dpm.enabled) {
        if (params->dpmq < 10) {
            fprintf(stderr, "%s: illegal dpmq = %d\n", path, params->dpmq);
            retval = FALSE;
        }
        if (!params->clock.enabled) {
            fprintf(stderr, "%s: DPM requires clock, but none given\n", path);
            retval = FALSE;
        } else {
            if (params->digitizer == ISP_DAS) {
                if (params->dpm.sint == 1) {
                    params->dpm.filt = 0;
                } else if (params->dpm.sint == 2) {
                    params->dpm.filt = 10;
                } else if (params->dpm.sint == 10) {
                    params->dpm.filt = 2;
                } else {
                    fprintf(stderr, "%s: dpm sint = %d != 1 | 2 | 10\n", 
                        path , params->dpm.sint
                    );
                    retval = FALSE;
                }
            }
        }
    } else {
        params->dpmq = 0;
    }

    if (params->digitizer == ISP_SAN) {
        if (params->san.timeout < 1) {
            fprintf(stderr, "%s: illegal SAN timeout = %d\n",
                path, params->san.timeout
            );
            retval = FALSE;
        }
        if (params->san.timeout < ISP_MIN_SAN_TIMEOUT) {
            params->san.timeout = ISP_MIN_SAN_TIMEOUT;
        }
        if (params->san.sohint < 1) {
            fprintf(stderr, "%s: illegal SAN SOH interval = %d\n",
                path, params->san.sohint
            );
            retval = FALSE;
        }
        if (params->san.sohint < ISP_MIN_SAN_SOHINT) {
            params->san.sohint = ISP_MIN_SAN_SOHINT;
        }
    }

    if (server->to < 1) {
        fprintf(stderr, "%s: illegal socketto = %d\n", path, server->to);
        retval = FALSE;
    }
    if (server->maxclients < 1) {
        fprintf(stderr, "%s: illegal maxclients = %d\n", path, server->maxclients);
        retval = FALSE;
    }
    if (server->port < DEFAULT_ISP_TCPPORT) {
        fprintf(stderr, "%s: illegal tcpport = %d\n", path, server->port);
        retval = FALSE;
    }

    return retval;
}

static BOOL LoadBarometerParams(int argc, char **argv, ISP_PARAMS *params)
{
int format;

    switch (params->digitizer) {
      case ISP_DAS:
        if (argc != 7 && argc != 8) return FALSE;
        format = 8;
        break;
      case ISP_SAN:
        if (argc != 6) return FALSE;
        format = 10;
        break;
      default:
        return FALSE;
    }

    strncpy(params->baro.port, argv[1], ISP_DEVNAMLEN);
    params->baro.port[ISP_DEVNAMLEN] = 0;
    params->baro.baud =       atoi(argv[2]);
    params->baro.flow = ttyio_atoi(argv[3]);
    if (format == 8) {
        params->baro.strm = atoi(argv[4]);
        params->baro.chan = atoi(argv[5]);
        params->baro.sint = atoi(argv[6]);
        params->baro.comp = FALSE;
    } else {
        strncpy(params->baro.name, argv[4], IDA10_CNAMLEN);
        params->baro.name[IDA10_CNAMLEN] = 0;
        params->baro.sint = atoi(argv[5]);
    }

    params->baro.pr = DEFAULT_ISP_PARO_PR;
    strncpy(params->baro.uf, DEFAULT_ISP_PARO_UF, ISP_MAXPAROPARAMLEN);
    params->baro.enabled = 1;

    return TRUE;
}

static BOOL LoadDPMParams(int argc, char **argv, ISP_PARAMS *params)
{
int format;

    switch (params->digitizer) {
      case ISP_DAS:
        if (argc != 8) return FALSE;
        format = 8;
        break;
      case ISP_SAN:
        if (argc != 7) return FALSE;
        format = 10;
        break;
      default:
        return FALSE;
    }

    strncpy(params->dpm.port, argv[1], ISP_DEVNAMLEN);
    params->dpm.port[ISP_DEVNAMLEN] = 0;
    params->dpm.baud =       atoi(argv[2]);
    params->dpm.flow = ttyio_atoi(argv[3]);
    if (format == 8) {
        params->dpm.strm   = atoi(argv[4]);
        params->dpm.chan   = atoi(argv[5]);
        params->dpm.sint   = atoi(argv[6]);
        params->dpm.thresh = atof(argv[7]);
    } else {
        strncpy(params->dpm.name, argv[4], IDA10_CNAMLEN);
        params->dpm.name[IDA10_CNAMLEN] = 0;
        params->dpm.sint   = atoi(argv[5]);
        params->dpm.thresh = atof(argv[6]);
    }
    params->dpm.multiplier = 100;
    params->dpm.enabled = 1;

    return TRUE;
}

static BOOL LoadClockParams(int argc, char **argv, ISP_PARAMS *params)
{
    if (argc != 6) return FALSE;
    strncpy(params->clock.port, argv[1], ISP_DEVNAMLEN);
    params->clock.port[ISP_DEVNAMLEN] = 0;
    params->clock.baud      =       atoi(argv[2]);
    params->clock.flow      = ttyio_atoi(argv[3]);
    params->clock.tolerance =       atoi(argv[4]);
    params->clock.type      =       atoi(argv[5]);
    params->clock.enabled = 1;

    return TRUE;
}

static BOOL LoadIbootParams(int argc, char **argv, ISP_PARAMS *params)
{
    if (argc != 6) return FALSE;
    params->iboot.server   = strdup(argv[1]);
    params->iboot.port     =   atoi(argv[2]);
    params->iboot.passwd   = strdup(argv[3]);
    params->iboot.interval =   atoi(argv[4]);
    params->iboot.maxcycle =   atoi(argv[5]);

    return TRUE;
}

BOOL ispLoadRunParam(char *path, char *sta, ISP_PARAMS *paramptr, ISP_SERVER *server)
{
BOOL first = TRUE;
BOOL ok;
FILE *fp;
int i, j, status, lineno = 0, ntoken, errors;
char *token[MAX_TOKEN];
char buffer[BUFLEN];
ISP_PARAMS unused, *params;
static ISP_IBOOT default_iboot = DEFAULT_ISP_IBOOT;
static char *fid = "load_param";

    if (paramptr != NULL) {
        params = paramptr;
    } else {
        params = &unused;
    }

/* Load default parameters */

    strcpy(params->port, DEFAULT_ISP_INPUT);
    strcpy(params->odev, DEFAULT_ISP_OUTPUT);

    params->ibaud         = DEFAULT_ISP_IBAUD;
    params->obaud         = DEFAULT_ISP_OBAUD;
    params->mtu           = DEFAULT_ISP_MTU;
    params->delay         = DEFAULT_ISP_DELAY;
    params->statint       = DEFAULT_ISP_STATINT;
    params->ttyto         = DEFAULT_ISP_TTYTO;
    params->nodatato      = DEFAULT_ISP_NODATATO;
    params->iddatato      = DEFAULT_ISP_IDDATATO;

    params->san.addr[0]   = 0;
    params->san.port.data = DEFAULT_ISP_SAN_DATA_PORT;
    params->san.port.cmnd = DEFAULT_ISP_SAN_CMND_PORT;
    params->san.timeout   = DEFAULT_ISP_SAN_TIMEOUT;
    params->san.sohint    = DEFAULT_ISP_SAN_SOHINT;

    params->rawq          = DEFAULT_ISP_RAWQ;
    params->cmdq          = DEFAULT_ISP_CMDQ;
    params->barq          = DEFAULT_ISP_BARQ;
    params->dpmq          = DEFAULT_ISP_DPMQ;

    params->bfact         = DEFAULT_ISP_BFACT;
    params->obuf          = DEFAULT_ISP_OBUF;
    params->numobuf       = DEFAULT_ISP_NUMOBUF;

    params->savedasstats  = 0;
    params->overwrite     = 0;
    params->nrts          = DEFAULT_ISP_NRTS;
    params->inject        = DEFAULT_ISP_INJECT;

    params->baro.enabled  = 0;
    params->dpm.enabled   = 0;
    params->clock.enabled = 0;

    params->flags         = DEFAULT_ISP_IDA_FLAGS;

    params->rt593.correct = DEFAULT_ISP_RT593;
    params->rt593.present = (params->rt593.correct) ? TRUE : FALSE;

    params->iboot         = default_iboot;

    server->sd         = -1;
    server->port       = DEFAULT_ISP_TCPPORT;
    server->to         = DEFAULT_ISP_SOCKETTO;
    server->maxclients = DEFAULT_ISP_MAXCLIENTS;

/* Override with contents of configuration file */

    if ((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, "%s: fopen", fid);
        perror(path);
        return FALSE;
    }

    while ((status = util_getline(fp, buffer, BUFLEN, '#', &lineno)) == 0) {
        ntoken = util_parse(buffer, token, DELIMITERS, MAX_TOKEN, 0);

    /* Check for SAN flag as first non-comment line */

        if (first) {
            if (ntoken == 2 && strcasecmp(token[0], "Digitizer") == 0) {
                params->digitizer = ISP_SAN;
                params->pktrev    = DEFAULT_ISP_SAN_PKTREV;
            } else {
                params->digitizer = ISP_DAS;
                params->pktrev    = DEFAULT_ISP_DAS_PKTREV;
            }
            first = FALSE;
            continue;
        }

    /* barometer, dpm, clock and iboot lines do not use 
     * the simple a = b form... check for these
     */

        if (ntoken != NUM_TOKEN) {
            if (strcasecmp(token[0], "baro") == 0) {
                ok = LoadBarometerParams(ntoken, token, params);
            } else if (strcasecmp(token[0], "dpm") == 0) {
                ok = LoadDPMParams(ntoken, token, params);
            } else if (strcasecmp(token[0], "clock") == 0) {
                ok = LoadClockParams(ntoken, token, params);
            } else if (strcasecmp(token[0], "iboot") == 0) {
                ok = LoadIbootParams(ntoken, token, params);
            }
            if (ok) continue;
            fprintf(stderr, "%s: syntax error near `%s' line %d\n",
                fid, path, lineno
            );
            fclose(fp);
            return FALSE;
        }

        if (strcasecmp(token[0], "port") == 0) {
            strncpy(params->port, token[1], ISP_DEVNAMLEN);
            params->port[ISP_DEVNAMLEN] = 0;
        } else if (strcasecmp(token[0], "ibaud") == 0) {
            params->ibaud = atol(token[1]);
        } else if (strcasecmp(token[0], "obaud") == 0) {
            params->obaud = atol(token[1]);
        } else if (strcasecmp(token[0], "mtu") == 0) {
            params->mtu = atol(token[1]);
        } else if (strcasecmp(token[0], "delay") == 0) {
            params->delay = atol(token[1]);
        } else if (strcasecmp(token[0], "pktrev") == 0) {
            params->pktrev = atoi(token[1]);
        } else if (strcasecmp(token[0], "statint") == 0) {
            params->statint = atol(token[1]);
        } else if (strcasecmp(token[0], "ttyto") == 0) {
            params->ttyto = atol(token[1]);
        } else if (strcasecmp(token[0], "nodatato") == 0) {
            params->nodatato = atol(token[1]);
        } else if (strcasecmp(token[0], "iddatato") == 0) {
            params->iddatato = atol(token[1]);

        } else if (strcasecmp(token[0], "rawq") == 0) {
            params->rawq = atol(token[1]);
        } else if (strcasecmp(token[0], "cmdq") == 0) {
            params->cmdq = atol(token[1]);
        } else if (strcasecmp(token[0], "barq") == 0) {
            params->barq = atol(token[1]);
        } else if (strcasecmp(token[0], "dpmq") == 0) {
            params->dpmq = atol(token[1]);
        } else if (strcasecmp(token[0], "rt593") == 0) {
            params->rt593.correct= atoi(token[1]);
            params->rt593.present = TRUE;

        } else if (strcasecmp(token[0], "output") == 0) {
            strncpy(params->odev, token[1], ISP_DEVNAMLEN);
            params->odev[ISP_DEVNAMLEN] = 0;
        } else if (strcasecmp(token[0], "bfact") == 0) {
            params->bfact = atol(token[1]);
        } else if (strcasecmp(token[0], "obuf") == 0) {
            params->obuf = atol(token[1]);
        } else if (strcasecmp(token[0], "numobuf") == 0) {
            params->numobuf = atol(token[1]);
        } else if (strcasecmp(token[0], "savedasstats") == 0) {
            params->savedasstats = atol(token[1]);
        } else if (strcasecmp(token[0], "nrts") == 0) {
            params->nrts = atoi(token[1]);
        } else if (strcasecmp(token[0], "inject") == 0) {
            params->inject = atoi(token[1]);

        } else if (strcasecmp(token[0], "dbdir") == 0 || (strcasecmp(token[0], "db") == 0)) {
            ; /* ignore db entries in run file */

        } else if (strcasecmp(token[0], "dbformat") == 0) {
            ; /* ignore db entries in run file */

        } else if (strcasecmp(token[0], "socketto") == 0) {
            server->to = atol(token[1]);

        } else if (strcasecmp(token[0], "tcpport") == 0) {
            server->port = atol(token[1]);

        } else if (strcasecmp(token[0], "maxclients") == 0) {
            server->maxclients = atol(token[1]);

        } else if (strcasecmp(token[0], "SANaddr") == 0) {
            memcpy(params->san.addr, token[1], ISP_ADDRNAMLEN);
            params->san.addr[ISP_ADDRNAMLEN] = 0;
            
        } else if (strcasecmp(token[0], "DataPort") == 0) {
            params->san.port.data = atoi(token[1]);

        } else if (strcasecmp(token[0], "CmndPort") == 0) {
            params->san.port.cmnd = atoi(token[1]);

        } else if (strcasecmp(token[0], "TimeOut") == 0) {
            params->san.timeout = atoi(token[1]);

        } else if (strcasecmp(token[0], "SohInterval") == 0) {
            params->san.sohint = atoi(token[1]);

        } else if (strcasecmp(token[0], "Flags") == 0) {
            params->flags = (UINT32) atoi(token[1]);

        } else {
            fprintf(stderr, "%s: syntax error near line %d of `%s'\n",
                fid, lineno, path
            );
            fprintf(stderr, "%s: unrecognized identifer `%s'\n",
                fid, token[0]
            );
            fclose(fp);
            return FALSE;
        }
    }
    fclose(fp);

    if (params == &unused) return TRUE;

    params->ida = idaCreateHandle(util_lcase(sta), params->pktrev, NULL, params->glob.db, NULL, params->flags);
    return CheckParam(sta, params, server, path);
}

void ispSetLogParameter(ISP_PARAMS *params, LOGIO *lp)
{
    if (params != NULL && params->ida != NULL) params->ida->lp = lp;
}

/* Revision History
 *
 * $Log: param.c,v $
 * Revision 1.12  2008/08/21 21:29:54  dechavez
 * 4.4.0
 *
 * Revision 1.11  2007/11/01 22:14:05  dechavez
 * fix addressing null params bug in ispSetLogParameter()
 *
 * Revision 1.10  2007/01/25 20:24:48  dechavez
 * RT593 support
 *
 * Revision 1.9  2006/02/09 20:05:14  dechavez
 * support for libida 4.0.0 and libisidb 1.0.0
 *
 * Revision 1.8  2005/09/06 18:27:31  dechavez
 * changed DEFAULT_ISP_TTYTO to 1
 *
 * Revision 1.7  2005/08/26 18:26:49  dechavez
 * Removed "capacity" paramter for CDR output (now that ISO creation is completely
 * decoupled from ISP functionality, modified various default paramters to match
 * what is used in practice.
 *
 * Revision 1.6  2004/09/29 18:13:25  dechavez
 * Check IDA_DBDIR environment for default MK7 dbspec, otherwise use /usr/nrts
 *
 * Revision 1.5  2004/04/26 20:57:07  dechavez
 * dbdir -> dpspec
 *
 * Revision 1.4  2002/09/12 16:06:35  dechavez
 * fixed error checking arg count in LoadBarometerParams()
 *
 * Revision 1.3  2002/09/10 17:52:58  dec
 * changed if statements for parsing barometer and DPM parameters to
 * test on packet format instead of digitizer
 *
 * Revision 1.2  2002/09/09 21:45:02  dec
 * added dpm parameters and cleaned up code using new LoadBarometerParams(),
 * LoadDPMParams(), and LoadClockParams()
 *
 * Revision 1.1  2002/03/15 22:45:08  dec
 * created
 *
 */
