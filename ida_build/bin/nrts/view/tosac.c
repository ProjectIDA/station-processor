#pragma ident "$Id: tosac.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <stdio.h>
#include <sys/param.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include "util.h"
#include "ida.h"
#include "nrts.h"
#include "sacio.h"
#include "null_sac.h"

#define BUFLEN 1024

struct chn_hist {
    char name[NRTS_CNAMLEN+1];
    char fname[NRTS_SNAMLEN + NRTS_CNAMLEN + 10];
    int nrec;
    int ident;
    FILE *fp;
    struct sac_header sac;
};

struct sta_hist {
    char name[NRTS_SNAMLEN+1];
    int nchn;
    struct chn_hist chn[NRTS_MAXCHN];
};
static struct sta_hist sta[NRTS_MAXSTA];

static int nsta = 0;
static int ascii;
struct sac_header *defaults;
            
int Nrts_tosac_wrthdr(fp, hdr, packet)
FILE *fp;
struct sac_header *hdr;
struct nrts_packet *packet;
{
int yr, da, hr, mn, sc, ms, chan_code = -1;
static char *fid = "Nrts_tosac_wrthdr";

/*  Initialize header if packet is given  */

    if (packet != NULL) {

        util_log(2, "create %s:%s SAC header", packet->sname, packet->cname);

        hdr->npts   = 0;
        hdr->delta  = packet->sint;
        hdr->b      = 0.0;

        util_tsplit(packet->beg.time, &yr, &da, &hr, &mn, &sc, &ms);
        hdr->nzyear = yr;
        hdr->nzjday = da;
        hdr->nzhour = hr;
        hdr->nzmin  = mn;
        hdr->nzsec  = sc;
        hdr->nzmsec = ms;

        util_ucase(strcpy(hdr->kstnm,  packet->sname));
        util_ucase(strcpy(hdr->kcmpnm, packet->cname));
 
        hdr->iftype = ITIME;
        hdr->leven  = TRUE;
        hdr->lpspol = TRUE;
        hdr->lovrok = TRUE;
        hdr->lcalda = TRUE;
        hdr->idep   = IUNKN;
        hdr->iztype = IB;

        switch (hdr->kcmpnm[0]) {
          case 'E':
          case 'S':
            switch (hdr->kcmpnm[2]) {
                case 'Z': chan_code = SPZ; break;
                case 'N': chan_code = SPN; break;
                case 'E': chan_code = SPE; break;
                default:  chan_code = -1;
            }
          default:
            switch (hdr->kcmpnm[2]) {
                case 'Z': chan_code = BBZ; break;
                case 'N': chan_code = BBN; break;
                case 'E': chan_code = BBE; break;
                default:  chan_code = -1;
            }
        }

    } else {
        util_log(2, "update %s:%s SAC header", hdr->kstnm, hdr->kcmpnm);
    }

    rewind(fp);

    if (ascii) {
        if (sacio_wah(fp, hdr) != 0) {
            util_log(1, "error writing SAC header: sacio_wah: %s",
                syserrmsg(errno)
            );
            return -1;
        }
    } else {
        if (sacio_wbh(fp, hdr) != 0) {
            util_log(1, "error writing SAC header: sacio_wbh: %s",
                syserrmsg(errno)
            );
            return -1;
        }
    }

    return 0;
}

struct chn_hist *Nrts_tosac_mkfile(packet, ident)
struct nrts_packet *packet;
int ident;
{
static struct chn_hist chn;
static char *fid = "Nrts_tosac_mkfile";

    strcpy(chn.name, packet->cname);
    chn.nrec = 0;
    if (ident) {
        sprintf(chn.fname, "%s.%s.%d", packet->sname, packet->cname, ident);
    } else {
        sprintf(chn.fname, "%s.%s", packet->sname, packet->cname);
    }
    util_lcase(chn.fname);
    util_log(2, "%s: create file %s", fid, chn.fname);
    if ((chn.fp = fopen(chn.fname, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, chn.fname, syserrmsg(errno));
        return NULL;
    }

    chn.sac = *defaults;
    chn.sac.npts = 0;
    if (Nrts_tosac_wrthdr(chn.fp, &chn.sac, packet) != 0) return NULL;

    return &chn;
}

struct chn_hist *Nrts_tosac_getchn(packet)
struct nrts_packet *packet;
{
FILE *fp;
int i, j, new;
struct chn_hist *chn;
static char *fid = "Nrts_tosac_getfp";

    for (i = 0; i < nsta; i++) {

        if (strcmp(sta[i].name, packet->sname) == 0) {

            for (j = 0; j < sta[i].nchn; j++) {
                chn = &(sta[i].chn[j]);
                if (strcmp(chn->name, packet->cname) == 0) {
                    if (packet->tear != 0.0) {
                        fclose(chn->fp);
                        chn = Nrts_tosac_mkfile(packet, chn->ident + 1);
                        if (chn == NULL) return NULL;
                        sta[i].chn[j] = *chn;
                    }
                    return chn;
                }
            }

        /*  Must be a new channel for this station... add it to the list */

            if (sta[i].nchn == NRTS_MAXCHN) {
                util_log(1, "%s: too many channels, increase NRTS_MAXCHN", fid);
                return NULL;
            }

            new = sta[i].nchn;
            if ((chn = Nrts_tosac_mkfile(packet, 0)) == NULL) return NULL;
            sta[i].chn[new] = *chn;
            ++sta[i].nchn;
            return chn;
        }
    }

/*  Must be a new station ... add it to the list  */

    if (nsta == NRTS_MAXSTA) {
        util_log(1, "%s: too many stations, increase NRTS_MAXSTA", fid);
        return NULL;
    }
   
    new = nsta;
    strcpy(sta[new].name, packet->sname);
    sta[new].nchn = 1;
    if ((chn = Nrts_tosac_mkfile(packet, 0)) == NULL) return NULL;
    sta[new].chn[0] = *chn;
    ++nsta;

    return sta[new].chn;
}

int Nrts_tosac_wrtdat(chn, packet)
struct chn_hist *chn;
struct nrts_packet *packet;
{
int i;
short *sdata;
long  *ldata;
float fdata[BUFLEN];
static char *fid = "Nrts_tosac_wrtdat";

    if (packet->nsamp > BUFLEN) {
        util_log(1, "%s: error: BUFLEN < %ld", fid, packet->nsamp);
        return -1;
    }

    if (packet->order != util_order()) {
        util_log(1, "%s: data byte order 0x%07x != host order 9x%07x",
            fid, packet->order, util_order()
        );
        return -2;
    }

    if (packet->wrdsiz == sizeof(short)) {
        sdata = (short *) packet->data;
        for (i = 0; i < packet->nsamp; i++) fdata[i] = (float) sdata[i];
    } else if (packet->wrdsiz == sizeof(long)) {
        ldata = (long *) packet->data;
        for (i = 0; i < packet->nsamp; i++) fdata[i] = (float) ldata[i];
    } else {
        util_log(1, "%s: error: wrdsiz `%d' is neither %d nor %d", 
            fid, packet->wrdsiz, sizeof(short), sizeof(long)
        );
        return -3;
    }

    if (++chn->nrec == 1) {
        chn->sac.depmin = fdata[0];
        chn->sac.depmax = fdata[0];
    }
    
    for (i = 0; i < packet->nsamp; i++) {
        if (fdata[i] < chn->sac.depmin) chn->sac.depmin = fdata[i];
        if (fdata[i] > chn->sac.depmax) chn->sac.depmax = fdata[i];
    }

    if (ascii) {
        return packet->nsamp -
               sacio_wad(chn->fp, fdata, packet->nsamp, &chn->sac.npts);
    } else {
        chn->sac.npts += packet->nsamp;
        return packet->nsamp -
               fwrite(fdata, sizeof(float), packet->nsamp, chn->fp);
    }

    return 0;
}

int Nrts_tosac(fp, doascii, hdrdefaults)
FILE *fp;
int doascii;
struct sac_header *hdrdefaults;
{
int i, j;
struct chn_hist *chn;
struct nrts_packet packet;
static char *fid = "Nrts_tosac";

    if (sizeof(unsigned long) != sizeof(float)) {
        util_log(1, "%s: error: sizeof(unsigned long) != sizeof(float)", fid);
        return -1;
    }

    ascii = doascii;
    packet.header = packet.data = NULL;
    if (hdrdefaults != NULL) {
        defaults = hdrdefaults;
    } else {
        defaults = &null_sac_header;
    }

    while (nrts_rdpacket(fp, &packet) == 0) {
        if ((chn = Nrts_tosac_getchn(&packet)) == NULL) {
            util_log(1, "%s: error: can't get output channel info", fid);
            return -2;
        }
        if (Nrts_tosac_wrtdat(chn, packet) != 0) {
            util_log(1, "%s: error: Nrts_tosac_wrtdat: %s", fid, syserrmsg(errno));
            return -3;
        }
    }

    util_log(2, "udpate headers and close files");
    for (i = 0; i < nsta; i++) {
        for (j = 0; j < sta[i].nchn; j++) {
            chn = sta[i].chn + j;
            if (Nrts_tosac_wrthdr(chn->fp, &chn->sac, NULL) != 0) return -4;
            fclose(chn->fp);
        }
    }

    return 0;
}

int Nrts_packettosac(packet, npackets, doascii, hdrdefaults)
struct nrts_packet *packet;
int npackets;
int doascii;
struct sac_header *hdrdefaults;
{
int i;
struct chn_hist *chn;
static char *fid = "Nrts_tosac";

	    if (sizeof(unsigned long) != sizeof(float)) {
 	       util_log(1, "%s: error: sizeof(unsigned long) != sizeof(float)", fid);
 	       return -1;
 	   	}
	
 	   	ascii = doascii;
			nsta = 0;
 	   	if (hdrdefaults != NULL) {
 	   	    defaults = hdrdefaults;
 	   	} else {
 	   	    defaults = &null_sac_header;
 	   	}

			for(i = 0; i < npackets; i++) {		
 		  	 if ((chn = Nrts_tosac_getchn(&packet[i])) == NULL) {
 		   	    util_log(1, "%s: error: can't get output channel info", fid);
 		   	      return -2;
 		   	}
 		   	if (Nrts_tosac_wrtdat(chn, &(packet[i])) != 0) {
 		   		   util_log(1, "%s: error: Nrts_tosac_wrtdat: %s", fid, syserrmsg(errno));
 		      	 return -3;
 		   	}
			}

    return Nrts_tosac_end();
}

int Nrts_tosac_end()
{
int i, j;
struct chn_hist *chn;

    util_log(2, "udpate headers and close files");
    for (i = 0; i < nsta; i++) {
        for (j = 0; j < sta[i].nchn; j++) {
            chn = &(sta[i].chn[j]);
            if (Nrts_tosac_wrthdr(chn->fp, &chn->sac, NULL) != 0) {
							return -4;
						}
            fclose(chn->fp);
						chn->fp = NULL;
        }
    }

		return 0;

}

/* Revision History
 *
 * $Log: tosac.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
