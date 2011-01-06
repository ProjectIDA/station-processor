#pragma ident "$Id: tocss.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <stdio.h>
#include <errno.h>
#include "util.h"
#include "ida.h"
#include "nrts.h"
#include "cssio.h"

#define WFDISC_FNAME "css30.wfdisc"

static char buffer[1024];

static FILE *wd = NULL;

struct info {
    FILE *fp;
    struct wfdisc wfdisc;
    long foff;
    int count;
};
static struct info master_info[NRTS_MAXSTA][NRTS_MAXCHN];

int Nrts_tocss_end();
struct wfdisc *Nrts_tocss_wfdisc();
struct info *Nrts_tocss_getinfo();
void Nrts_tocss_logpacket();
int Nrts_tocss_wrtpacket();
int Nrts_tocss();
int Nrts_packettocss();

int Nrts_tocss_end(status)
int status;
{
int i, j;
struct info *info;
static char *fid = "Nrts_tocss_end";

    for (i = 0; i < NRTS_MAXSTA; i++) {
        for (j = 0; j < NRTS_MAXCHN; j++) {
            info = &master_info[i][j];
            if (info->fp != NULL) {
                fclose(info->fp);
                if (wwfdisc(wd, &info->wfdisc) != 0) {
                    util_log(1, "%s: wwfdisc: %s: %s",
                        fid, WFDISC_FNAME, syserrmsg(errno)
                    );
                    return -100 - status;
                }
            }
        }
    }

    fclose(wd);
		wd = NULL;

    return status;
}

struct wfdisc *Nrts_tocss_wfdisc(packet)
struct nrts_packet *packet;
{
static struct wfdisc wfdisc;
int yr, da, hr, mn, sc, ms;

    wfdisc = wfdisc_null;
    strcpy(wfdisc.sta,     packet->sname);
    strcpy(wfdisc.chan,    packet->cname);
    strcpy(wfdisc.instype, packet->instype);
    wfdisc.time    = packet->beg.time;
    util_tsplit(wfdisc.time, &yr, &da, &hr, &mn, &sc, &ms);
    wfdisc.jdate   = (yr * 1000) + da;

    if (packet->order == BIG_ENDIAN_ORDER) {
        sprintf(wfdisc.datatype, "s%d", packet->wrdsiz);
    } else if (packet->order == LTL_ENDIAN_ORDER) {
        sprintf(wfdisc.datatype, "i%d", packet->wrdsiz);
    }

    wfdisc.nsamp   = packet->nsamp;
    wfdisc.smprate = 1.0 / packet->sint;
    wfdisc.calib   = packet->calib;
    wfdisc.calper  = packet->calper;
    strcpy(wfdisc.dir, ".");
    sprintf(wfdisc.dfile, "%s-%s.w", wfdisc.sta, wfdisc.chan);

    wfdisc.foff = 0;

    return &wfdisc;
}

struct info *Nrts_tocss_getinfo(packet)
struct nrts_packet *packet;
{
int i, j;
struct info *info;
static char *fid = "Nrts_tocss_getinfo";

/* See if data for this station/channel have already been seen  */

    for (i = 0; i < NRTS_MAXSTA; i++) {
        for (j = 0; j < NRTS_MAXCHN; j++) {
            if (master_info[i][j].fp != NULL) {
                info = &master_info[i][j];
                if (strcmp(packet->sname, info->wfdisc.sta) == 0) {
                    if (strcmp(packet->cname, info->wfdisc.chan) == 0) {
                        return info;
                    }
                }
            }
        }
    }

/*  Must not have seen it yet... add it to the list  */

    for (i = 0; i < NRTS_MAXSTA; i++) {
        for (j = 0; j < NRTS_MAXCHN; j++) {
            if (master_info[i][j].fp == NULL) {
                info = &master_info[i][j];
                info->wfdisc = *Nrts_tocss_wfdisc(packet);
                if ((info->fp = fopen(info->wfdisc.dfile, "w")) == NULL) {
                    util_log(1, "%s: fopen: %s: %s",
                        fid, info->wfdisc.dfile, syserrmsg(errno)
                    );
                    return NULL;
                }
                return info;
            }
        }
    }

/*  This should never happen  */

    util_log(1, "%s: no free info slots! This error should never occur!", fid);
    return NULL;
}

void Nrts_tocss_logpacket(count, packet)
long count;
struct nrts_packet *packet;
{
    sprintf(buffer, "%5d", ++count);
    sprintf(buffer+strlen(buffer), " %-5s",   packet->sname);
    sprintf(buffer+strlen(buffer), " %-4s",   packet->cname);
    sprintf(buffer+strlen(buffer), " %s", util_dttostr(packet->beg.time, 0));
    sprintf(buffer+strlen(buffer), " (`%c',", packet->beg.code);
    sprintf(buffer+strlen(buffer), " %2hd)",  packet->beg.qual);
    sprintf(buffer+strlen(buffer), " %6.3f",  packet->sint);
    sprintf(buffer+strlen(buffer), " %3d",    packet->nsamp);
    sprintf(buffer+strlen(buffer), " %2d",    packet->wrdsiz);
    sprintf(buffer+strlen(buffer), " %9.3f",  packet->tear);
    util_log(3, buffer);
}

int Nrts_tocss_wrtpacket(packet)
struct nrts_packet *packet;
{
long nbytes;
struct info *info;
static char *fid =  "Nrts_tocss_wrtpacket";

    if ((info = Nrts_tocss_getinfo(packet)) == NULL) return -10;
    if (info->count++ > 0) {
        if (packet->tear != 0.0) {
            util_log(2, "%.2f second %s:%s time jump",
                packet->tear, packet->sname, packet->cname
            );
            if (wwfdisc(wd, &info->wfdisc) != 0) {
                util_log(1, "%s: wwfdisc: %s: %s",
                    fid, WFDISC_FNAME, syserrmsg(errno)
                );
                return -20;
            }
            info->wfdisc = *Nrts_tocss_wfdisc(packet);
            info->wfdisc.foff = info->foff;
        } else {
            info->wfdisc.nsamp  += packet->nsamp;
            info->wfdisc.endtime = packet->end.time;
            info->foff += packet->dlen;
        }
    }

    nbytes = fwrite(packet->data, sizeof(char), packet->dlen, info->fp);
    if (nbytes != packet->dlen) {
        util_log(1, "%s: fwrite: %s: %s", 
            fid, info->wfdisc.dfile, syserrmsg(errno)
        );
        return -30;
    }

    return 0;
}

int Nrts_packettocss(packet, npackets)
struct nrts_packet *packet;
int npackets;
{
int i, j, status;
static long count = 0;
static char *fid = "Nrts_tocss";

		if(wd == NULL) {
    	for (i = 0; i < NRTS_MAXSTA; i++) {
     	   for (j = 0; j < NRTS_MAXCHN; j++) {
     	       master_info[i][j].fp    = NULL;
     	       master_info[i][j].foff  = 0;
     	       master_info[i][j].count = 0;
    	    }
    	}

    	if ((wd = fopen(WFDISC_FNAME, "w")) == NULL) {
     	   util_log(1, "%s: fopen: %s: %s", fid, WFDISC_FNAME, syserrmsg(errno));
     	   return (-1);
    	}
		}

		for(i = 0; i < npackets; i++) {
			Nrts_tocss_logpacket(++count, &(packet[i]));
			if ((status = Nrts_tocss_wrtpacket(&(packet[i]))) != 0) {
				util_log(1, "%s: tocss_wrtpacket: %s", fid, syserrmsg(errno));
				return status;
 	 	  }
		}

 	 	return status;
}
/*
int Nrts_packettocss(packet)
struct nrts_packet *packet;
{
int i, j, status;
static long count = 0;
static char *fid = "Nrts_tocss";

		if(wd == NULL) {
    	for (i = 0; i < NRTS_MAXSTA; i++) {
     	   for (j = 0; j < NRTS_MAXCHN; j++) {
     	       master_info[i][j].fp    = NULL;
     	       master_info[i][j].foff  = 0;
     	       master_info[i][j].count = 0;
    	    }
    	}

    	if ((wd = fopen(WFDISC_FNAME, "w")) == NULL) {
     	   util_log(1, "%s: fopen: %s: %s", fid, WFDISC_FNAME, syserrmsg(errno));
     	   return (-1);
    	}
		}

    Nrts_tocss_logpacket(++count, packet);
    if ((status = Nrts_tocss_wrtpacket(packet)) != 0) {
        return status;
    }

    return 0;
}
*/

/* Revision History
 *
 * $Log: tocss.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
