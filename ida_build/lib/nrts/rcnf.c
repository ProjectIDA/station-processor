#pragma ident "$Id: rcnf.c,v 1.11 2007/01/07 17:51:59 dechavez Exp $"
/*======================================================================
 *
 *  Read an NRTS configuration file
 *
 *====================================================================*/
#include "nrts.h"

#define DELIMITERS " ,"
#define MAXTEXT 128
#define COMMENT '#'
#define MAXTOKEN 16
#define GETLINE util_getline(fp, buffer, MAXTEXT, '#', &lineno)

static FILE *fp;
static int ntoken, status, lineno;
static char *name;
static char buffer[MAXTEXT+1];
static char *token[MAXTOKEN];

/*  Routine to read/set IDA specific parameters  */

static int ida_info(struct nrts_sys *sys)
{
int i, j, dlen, hlen;
size_t  slen = sizeof(u_short);
static struct nrts_ida ida;

/*  If nsta is not yet defined then we just need to read the configuration  */

    if (sys->nsta == 0) {

        assert(sizeof(struct nrts_ida) <= NRTS_MAXINFO);

        /* get data format revision */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse ida `rev' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            ida.rev = atoi(token[0]);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

    /* get channel map */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse ida `map' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            if (strlen(token[0]) > IDA_MAXMAPLEN) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"ida map name `%s' is too long, ", token[0]);
                fprintf(stderr,"increase IDA_MAXMAPLEN\n");
                return -1;
            }
            strlcpy(ida.map, token[0], IDA_MAXMAPLEN+1);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

    /* get shift (aka corrupt data) flag */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse ida `map' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            ida.shift = atoi(token[0]);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

    /*  copy structure into info field  */

        memcpy(sys->info, &ida, sizeof(struct nrts_ida));

        return 0;
    }

/*  The next time we are called we need to set all the sta/chan stuff  */

    sys->reclen = IDA_BUFSIZ;
    for (i = 0; i < sys->nsta; i++) {
        hlen = (ida.rev >= 5 && ida.rev <= 8) ? 60 : 64;
        dlen = 960;
        for (j = 0; j < sys->sta[i].nchn; j++) {
            sys->sta[i].chn[j].hdr.len = hlen;
            sys->sta[i].chn[j].dat.len = dlen;
        }
    }

    return 0;
}

/*  Routine to read/set ASL specific parameters  */

static int asl_info(struct nrts_sys *sys)
{
int i, j, dlen, hlen;
size_t  slen = sizeof(u_short);
static struct nrts_asl asl;

/*  If nsta is not yet defined then we just need to read the configuration  */

    if (sys->nsta == 0) {

        assert(sizeof(struct nrts_asl) <= NRTS_MAXINFO);

        /* get record size */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse asl `blksiz' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            asl.blksiz = atoi(token[0]);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

    /* get channel map */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse asl `map' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            if (strlen(token[0]) > ASL_MAXMAPLEN) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"asl map name `%s' is too long, ", token[0]);
                fprintf(stderr,"increase ASL_MAXMAPLEN\n");
                return -1;
            }
            strlcpy(asl.map, token[0], ASL_MAXMAPLEN+1);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

        /* get packet reformat flag */

        if ((status = GETLINE) == 0) {
            if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
                fprintf(stderr,"nrts configuration file error: ");
                fprintf(stderr,"can't parse asl `reformat' field, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -1;
            }
            asl.reformat = atoi(token[0]);
        } else if (status == 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"unexpected EOF, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        } else {
            fprintf(stderr,"nrts configuration file error: read error, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -1;
        }

    /*  copy structure into info field  */

        memcpy(sys->info, &asl, sizeof(struct nrts_asl));

        return 0;
    }

/*  The next time we are called we need to set all the sta/chan stuff  */

    sys->reclen = asl.blksiz;
    for (i = 0; i < sys->nsta; i++) {
        hlen = 64;
        dlen = asl.blksiz - hlen;
        for (j = 0; j < sys->sta[i].nchn; j++) {
            sys->sta[i].chn[j].hdr.len = hlen;
            sys->sta[i].chn[j].dat.len = dlen;
        }
    }

    return 0;
}

/*  Routine to read/set IDA10 specific parameters  */

static int ida10_info(struct nrts_sys *sys)
{
static int buflen=IDA10_DEFDATALEN;
int i, j;

/* First time we are called, set the record length */

    if (sys->nsta == 0) {
        if (ntoken > 1) buflen=atoi(token[1]);
        sys->reclen = IDA10_TSHEADLEN + buflen;
        return 0;
    }

/* Second time we are called, apply the lengths to all sta/chans */

    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            sys->sta[i].chn[j].hdr.len = IDA10_TSHEADLEN;
            sys->sta[i].chn[j].dat.len = buflen;
        }
    }
    return 0;
}

int nrts_rcnf(ffp, fname, sys)
FILE *ffp;
char *fname;
struct nrts_sys *sys;
{
int nsta = 0;
int nchn = 0;
int done;
int (*syspecific)();

    name = fname;
    fp   = ffp;
    rewind(fp);

/*  get system type flag    */

    if ((status = GETLINE) == 0) {
        if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) < 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"can't parse `type' field, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -3;
        }
        if ((sys->type = nrts_sysident(token[0])) == 0) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"`%s' is not a supported system type\n", token[0]);
            return -4;
        }
    } else if (status == 1) {
        fprintf(stderr,"nrts configuration file error: ");
        fprintf(stderr,"unexpected EOF, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -5;
    } else {
        fprintf(stderr,"nrts configuration file error: read error, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -6;
    }

/*  next set of file entries depends on the type of system  */

    sys->nsta = 0;
    switch (sys->type) {
      case NRTS_IDA:
        syspecific = ida_info;
        break;
      case NRTS_ASL:
        syspecific = asl_info;
        break;
      case NRTS_IDA10:
        syspecific = ida10_info;
        break;
      default:
        fprintf(stderr,"No parse code for system type `%s'\n", token[0]);
        return -7;
    }

    if ((*syspecific)(sys, token, ntoken) != 0) return -8;

/*  number of raw records to buffer  */

    if ((status = GETLINE) == 0) {
        if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 2) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"can't parse `raw' field, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -9;
        }
    } else if (status == 1) {
        fprintf(stderr,"nrts configuration file error: ");
        fprintf(stderr,"unexpected EOF, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -10;
    } else {
        fprintf(stderr,"nrts configuration file error: read error, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -11;
    }

/*  read in first station code  */

    if ((status = GETLINE) == 0) {
        if ((ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN)) != 1) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"can't parse `sta' field, ");
            fprintf(stderr,"%s line %d\n", name, lineno);
            return -9;
        }
    } else if (status == 1) {
        fprintf(stderr,"nrts configuration file error: ");
        fprintf(stderr,"unexpected EOF, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -10;
    } else {
        fprintf(stderr,"nrts configuration file error: read error, ");
        fprintf(stderr,"%s line %d\n", name, lineno);
        return -11;
    }
    done = 0;

    nsta = 0;
    while (!done) {

    /*  get station name  */

           if (strlen(token[0]) > NRTS_SNAMLEN) {
               fprintf(stderr,"nrts configuration file error: ");
               fprintf(stderr,"station name `%s' is too long, ", token[0]);
               fprintf(stderr,"increase NRTS_SNAMLEN\n");
               return -12;
           }

        if (nsta == NRTS_MAXSTA) {
            fprintf(stderr,"nrts configuration file error: ");
            fprintf(stderr,"too many stations specified, ");
            fprintf(stderr,"increase NRTS_MAXSTA\n");
            return -13;
        }
        strlcpy(sys->sta[nsta].name, token[0], NRTS_SNAMLEN+1);

        nchn = 0;
        do {
            ntoken = 0;
            if ((status = GETLINE) == 0) {
                ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN);
                if (ntoken == 3) {
                    if (nchn == NRTS_MAXCHN) {
                        fprintf(stderr,"nrts configuration file error: ");
                        fprintf(stderr,"too many channels specified, for ");
                        fprintf(stderr,"station `%s', ", sys->sta[nsta].name);
                        fprintf(stderr,"increase NRTS_MAXCHN\n");
                        return -14;
                    }

                /*  channel name  */

                    if (strlen(token[0]) > NRTS_CNAMLEN) {
                        fprintf(stderr,"nrts configuration file error: ");
                        fprintf(stderr,"channel name `%s' is too ", token[0]);
                        fprintf(stderr,"long, increase NRTS_CNAMLEN\n");
                        return -15;
                    }
                    strlcpy(sys->sta[nsta].chn[nchn].name,token[0], NRTS_CNAMLEN+1);
        
                /*  disk loop length  */
        
                    if (!util_isinteger(token[1])) {
                        fprintf(stderr,"nrts configuration file error: ");
                        fprintf(stderr,"illegal `nrec' field, ");
                        fprintf(stderr,"%s line %d\n", name, lineno);
                        return -16;
                    }
                    sys->sta[nsta].chn[nchn].hdr.nrec =
                    sys->sta[nsta].chn[nchn].dat.nrec = atol(token[1]);
        
                /*  number of "hidden" records  */
        
                    if (!util_isinteger(token[2])) {
                        fprintf(stderr,"nrts configuration file error: ");
                        fprintf(stderr,"illegal `hide' field, ");
                        fprintf(stderr,"%s line %d\n", name, lineno);
                        return -17;
                    }
                    sys->sta[nsta].chn[nchn].hdr.hide =
                    sys->sta[nsta].chn[nchn].dat.hide = atol(token[2]);
                    ++nchn;
                } else if (ntoken == 1) {
                    sys->sta[nsta].nchn = nchn;
                } else {
                    fprintf(stderr,"can't parse `sta' field, ");
                    fprintf(stderr,"%s line %d\n", name, lineno);
                    return -18;
                }
            } else if (status == 1) {
                sys->sta[nsta].nchn = nchn;
                done = 1;
            } else {
                fprintf(stderr,"nrts configuration file error: read error, ");
                fprintf(stderr,"%s line %d\n", name, lineno);
                return -19;
            }
        } while (ntoken == 3);
        ++nsta;
    }
    sys->nsta = nsta;

/*  Get system specific details, now that we have the file all read  */

    if ((*syspecific)(sys) != 0) return -20;

    return 0;
}

/* Revision History
 *
 * $Log: rcnf.c,v $
 * Revision 1.11  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.10  2006/02/09 19:44:23  dechavez
 * removed unneeded includes
 *
 * Revision 1.9  2005/08/26 18:19:02  dechavez
 * added ida rev 9 support
 *
 * Revision 1.8  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.7  2005/05/06 22:17:46  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.6  2004/06/24 17:37:07  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.5  2002/11/20 01:05:06  dechavez
 * removed tabs
 *
 * Revision 1.4  2002/03/15 22:46:07  dec
 * changed IDA10_HEADLEN to more specific IDA10_TSHEADLEN
 *
 * Revision 1.3  2001/09/09 01:11:39  dec
 * allow for specification of IDA10 data buffer length
 *
 * Revision 1.2  2001/02/19 01:15:32  dec
 * preserve case of station and channel names
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
