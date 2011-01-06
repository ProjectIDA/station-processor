#pragma ident "$Id: togse.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
/*======================================================================
 *
 *  Given a FILE of cooked NRTS data, demultiplex it to CM6 or CM8 format
 *  in the current directory.
 *
 *  Works by first unpacking all the data into files with names of the
 *
 *  form r.sta.chan[.n] where sta.chan are the (lower case) station and
 *  channel names.  If there are more than one output files for the
 *  same station and channel, then subsequent files are given the .n
 *  appendix, where n=1,2,3... etc.  Any pre-existing files in the
 *  current directory with the same names are silently overwritten.
 *
 *  These files are then mmap'd to arrays and the GSE2.0 checksum is
 *  computed and then compressed into files with names of the form
 *  c.sta.chan[.n] (using mmap), which observe the requested maximum line
 *  length.
 *
 *  Finally, the 
 *
 *  The [rc].sta.chan[.n] files are deleted upon completion.
 *
 *
 *====================================================================*/
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#include "nrts.h"

struct gse_header {
    double tofs;                  /* time of first sample         */
    char chn[NRTS_SNAMLEN+1];     /* station name                 */
    char sta[NRTS_CNAMLEN+1];     /* channel name                 */
    char auxid[5];                /* aux ident code (unused here) */
    char datatype[4];             /* data type code               */
    long nsamp;                   /* number of samples            */
    float srate;                  /* nominal sample rate (Hz)     */
    float calib;                  /* calib, nm/counts             */
    float calper;                 /* calibration reference period */
    char instype[NRTS_INAMLEN+1]; /* instrument type              */
    float hang;                   /* horizontal orientation angle */
    float vang;                   /* vertical orientation angle   */
    long checksum;                /* checksum                     */
};

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#define CMXBUFLEN 1024
static char gsebuf[CMXBUFLEN];

static int format;
static int nsta = 0;

struct chn_hist {
    char name[NRTS_CNAMLEN+1];
    char rawfile[NRTS_SNAMLEN + NRTS_CNAMLEN + 10];
    char cmpfile[NRTS_SNAMLEN + NRTS_CNAMLEN + 10];
    char outfile[NRTS_SNAMLEN + NRTS_CNAMLEN + 10];
    int nrec;
    int ident;
    FILE *raw;
    struct gse_header gse;
    struct {
        long *raw; /* mmap'd to raw disk file */
        char *cmp; /* mmap'd to compressed disk file */
    } data;
};

struct sta_hist {
    char name[NRTS_SNAMLEN+1];
    int nchn;
    struct chn_hist chn[NRTS_MAXCHN];
};

static struct sta_hist sta[NRTS_MAXSTA];

void Nrts_togse_inihdr();
struct chn_hist *Nrts_togse_mkfile();
struct chn_hist *Nrts_togse_getchn();

void Nrts_togse_inihdr(hdr, packet)
struct gse_header *hdr;
struct nrts_packet *packet;
{
static char *fid = "Nrts_togse_inihdr";

    util_log(2, "create %s:%s GSE header", packet->sname, packet->cname);

    hdr->nsamp  = 0;
    hdr->srate  = 1.0 / packet->sint;
    hdr->tofs   = packet->beg.time;
    hdr->calib  = packet->calib;
    hdr->calper = packet->calper;
    hdr->hang   = packet->hang;
    hdr->vang   = packet->vang;
    util_ucase(strcpy(hdr->sta,     packet->sname));
    util_ucase(strcpy(hdr->chn,     packet->cname));
    util_ucase(strcpy(hdr->instype, packet->instype));
    strcpy(hdr->auxid, "    ");
    hdr->checksum = 0;
        
}
            
int Nrts_togse_wrthdr(fp, hdr, linlen)
FILE *fp;
struct gse_header *hdr;
int linlen;
{
int yr, jd, mo, da, hr, mn, sc, ms;
static char *fid = "Nrts_togse_wrthdr";

    util_log(2, "write %s:%s GSE header", hdr->sta, hdr->chn);

/*  Build the header string  */

    util_tsplit(hdr->tofs, &yr, &jd, &hr, &mn, &sc, &ms);
    util_jdtomd(yr, jd, &mo, &da);

    sprintf(gsebuf, "WID2");
    sprintf(gsebuf+strlen(gsebuf), " %04d/%02d/%02d", yr, mo, da);
    sprintf(gsebuf+strlen(gsebuf), " %02d:%02d:%02d.%03d", hr, mn, sc, ms);
    sprintf(gsebuf+strlen(gsebuf), " %5s", hdr->sta);
    sprintf(gsebuf+strlen(gsebuf), " %3s", hdr->chn);
    sprintf(gsebuf+strlen(gsebuf), " %4s", hdr->auxid);
    if (format == NRTS_CM6) {
        sprintf(gsebuf+strlen(gsebuf), " CM6");
    } else {
        sprintf(gsebuf+strlen(gsebuf), " CM8");
    }
    sprintf(gsebuf+strlen(gsebuf), " %8ld",   hdr->nsamp);
    sprintf(gsebuf+strlen(gsebuf), " %11.6f", hdr->srate);
    sprintf(gsebuf+strlen(gsebuf), " %10.2e", hdr->calib);
    sprintf(gsebuf+strlen(gsebuf), " %7.3f",  hdr->calper);
    sprintf(gsebuf+strlen(gsebuf), " %6s",    hdr->instype);
    sprintf(gsebuf+strlen(gsebuf), " %5.1f",  hdr->hang);
    sprintf(gsebuf+strlen(gsebuf), " %4.1f",  hdr->hang);
    sprintf(gsebuf+strlen(gsebuf), "\n");
    sprintf(gsebuf+strlen(gsebuf), "DAT2\n");

/*  Print the header  */

    if ((util_lenprt(fp, gsebuf, linlen, '\\')) != 0) {
        util_log(1, "error writing GSE header: util_prtline: %s",
            syserrmsg(errno)
        );
        return -1;
    }

    return 0;
}

struct chn_hist *Nrts_togse_mkfile(packet, ident)
struct nrts_packet *packet;
int ident;
{
static struct chn_hist chn;
static char *fid = "Nrts_togse_mkfile";

    strcpy(chn.name, packet->cname);
    chn.nrec = 0;
    if (ident) {
        sprintf(chn.rawfile, "r.%s.%s.%d", packet->sname, packet->cname, ident);
        sprintf(chn.cmpfile, "c.%s.%s.%d", packet->sname, packet->cname, ident);
        sprintf(chn.outfile,   "%s.%s.%d", packet->sname, packet->cname, ident);
    } else {
        sprintf(chn.rawfile, "r.%s.%s", packet->sname, packet->cname);
        sprintf(chn.cmpfile, "c.%s.%s", packet->sname, packet->cname);
        sprintf(chn.outfile,   "%s.%s", packet->sname, packet->cname);
    }
    util_lcase(chn.rawfile);
    util_lcase(chn.cmpfile);
    util_lcase(chn.outfile);
    util_log(2, "%s: create file %s", fid, chn.rawfile);
    if ((chn.raw = fopen(chn.rawfile, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, chn.rawfile, syserrmsg(errno));
        return NULL;
    }

    Nrts_togse_inihdr(&chn.gse, packet);

    return &chn;
}

struct chn_hist *Nrts_togse_getchn(packet)
struct nrts_packet *packet;
{
FILE *fp;
int i, j, new;
struct chn_hist *chn;
static char *fid = "Nrts_togse_getchn";

    for (i = 0; i < nsta; i++) {

        if (strcmp(sta[i].name, packet->sname) == 0) {

            for (j = 0; j < sta[i].nchn; j++) {
                chn = sta[i].chn + j;
                if (strcmp(chn->name, packet->cname) == 0) {
                    if (packet->tear != 0.0) {
                        fclose(chn->raw);
                        chn->raw = NULL;
                        chn = Nrts_togse_mkfile(packet, chn->ident + 1);
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
            if ((chn = Nrts_togse_mkfile(packet, 0)) == NULL) return NULL;
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
    if ((chn = Nrts_togse_mkfile(packet, 0)) == NULL) return NULL;
    sta[new].chn[0] = *chn;
    ++nsta;

    return sta[new].chn;
}

int Nrts_togse_Rwrtdat(chn, packet)
struct chn_hist *chn;
struct nrts_packet *packet;
{
int i;
short *sdata;
long  *ldata;
long  ltmp[CMXBUFLEN];
static char *fid = "Nrts_togse_Rwrtdat";

    if (packet->nsamp > CMXBUFLEN) {
        util_log(1, "%s: error: CMXBUFLEN < %ld", fid, packet->nsamp);
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
        for (i = 0; i < packet->nsamp; i++) ltmp[i] = (long) sdata[i];
    } else if (packet->wrdsiz == sizeof(long)) {
        ldata = (long *) packet->data;
    } else {
        util_log(1, "%s: error: wrdsiz `%d' is neither %d nor %d", 
            fid, packet->wrdsiz, sizeof(short), sizeof(long)
        );
        return -3;
    }

    chn->gse.nsamp += packet->nsamp;

    if (fwrite(ldata, sizeof(long), packet->nsamp, chn->raw) != packet->nsamp) {
        return -4;
    }

    return 0;
}

int Nrts_packettogse(packet, npackets, code, linlen, diff)
struct nrts_packet *packet;
int npackets;
int code;
int linlen;
int diff;
{
int i;
struct chn_hist *chn;
static char *fid = "Nrts_packettogse";

    if (code == NRTS_CM6 || code == NRTS_CM8) {
        format = code;
    } else {
        errno = EINVAL;
        return -1;
    }
        
/*  demultiplex raw data to disk  */

    nsta = 0;
		for(i = 0; i < npackets; i++) {
        if ((chn = Nrts_togse_getchn(&packet[i])) == NULL) {
            util_log(1, "%s: error: can't get output channel info", fid);
            return -2;
        }
        if (Nrts_togse_Rwrtdat(chn, &packet[i]) != 0) {
            util_log(1, "%s: error: togse_Rwrtdat: %s", fid, syserrmsg(errno));
            return -3;
        }
    }

		Nrts_togse_end(linlen, diff);

}

int Nrts_togse_end(linlen, diff)
int linlen;
int diff;
{
FILE *cmp, *out;
int i, j, k, fd, flag, prot;
char *addr;
off_t off;
size_t len;
long nbytes;
struct chn_hist *chn;
static char *fid = "Nrts_togse_end";


/*  Close all raw files  */

    for (i = 0; i < nsta; i++) {
        for (j = 0; j < sta[i].nchn; j++) {
            chn = sta[i].chn + j;
            if (chn->raw != NULL) fclose(chn->raw);
        }
    }

/*  Process each raw data file in turn  */

    for (i = 0; i < nsta; i++) {
        for (j = 0; j < sta[i].nchn; j++) {
            chn = sta[i].chn + j;

    /*  Open all files for this chn  */

            if ((chn->raw = fopen(chn->rawfile, "r+")) == NULL) {
                util_log(1, "%s: fopen: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -4;
            }

            if ((cmp = fopen(chn->cmpfile, "w+")) == NULL) {
                util_log(1, "%s: fopen: %s: %s",
                    fid, chn->cmpfile, syserrmsg(errno)
                );
                return -5;
            }

            if ((out = fopen(chn->outfile, "w")) == NULL) {
                util_log(1, "%s: fopen: %s: %s",
                    fid, chn->cmpfile, syserrmsg(errno)
                );
                return -6;
            }

    /* mmap raw data file to array  */

            addr = (char *) 0;
            fd = fileno(chn->raw);
            if ((len = lseek(fd, 0, SEEK_END)) < 0) {
                util_log(1, "%s: lseek: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -5;
            }
            prot = PROT_READ | PROT_WRITE;
            flag = (int)    MAP_FILE | MAP_SHARED;
            off  = (off_t)  0;

            chn->data.raw = (long *) mmap(addr, len, prot, flag, fd, off);

            if (chn->data.raw == (long *) -1) {
                util_log(1, "%s: mmap: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -6;
            }

    /* create/size compressed data file, and mmap to array */

            fd = fileno(cmp);
            if (lseek(fd, len - 1, SEEK_SET) < 0) {
                util_log(1, "%s: lseek: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -10;
            }
            if (write(fd, chn->data.raw, 1) != 1) {
                util_log(1, "%s: lseek: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -11;
            }
            if (lseek(fd, 0, SEEK_SET) < 0) {
                util_log(1, "%s: lseek: %s: %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
                return -12;
            }

            addr = (char *) 0;
            prot = PROT_READ | PROT_WRITE;
            flag = (int) MAP_FILE | MAP_SHARED;
            off  = (off_t)  0;

            chn->data.cmp = (char *) mmap(addr, len, prot, flag, fd, off);

            if (chn->data.cmp == (char *) -1) {
                util_log(1, "%s: mmap: %s: %s",
                    fid, chn->cmpfile, syserrmsg(errno)
                );
                return -13;
            }

    /* compute GSE 2.0 checksum  */

            chn->gse.checksum = util_chksum(chn->data.raw, chn->gse.nsamp);

    /* apply CMX compression */

            if (format == NRTS_CM6) {
                nbytes = util_cm6(
                    chn->data.raw, chn->data.cmp, chn->gse.nsamp, len, diff, 0);
                if (nbytes < 0) {
                    util_log(1, "%s: util_cm6: %s: %s",
                        fid, chn->cmpfile, syserrmsg(errno)
                    );
                    return -14;
                }
            } else {
                nbytes = util_cm8(
                    chn->data.raw, chn->data.cmp, chn->gse.nsamp, len, diff, 0);
                if (nbytes < 0) {
                    util_log(1, "%s: util_cm8: %s: %s",
                        fid, chn->cmpfile, syserrmsg(errno)
                    );
                    return -14;
                }
            }

    /* write CMX header */

            if (Nrts_togse_wrthdr(out, &chn->gse, linlen) != 0) return -8;

    /* write compressed data */

            util_log(2, "write compressed data to %s", chn->outfile);
            if (format == NRTS_CM6) {
                for (k = 0; k < nbytes; k++) {
                    if (k && k % linlen == 0) fprintf(out, "\n");
                    fprintf(out, "%c", chn->data.cmp[k]);
                }
                if (k % linlen) fprintf(out, "\n");
            } else {
                if (fwrite(chn->data.cmp, sizeof(char), nbytes, out) != nbytes){
                    util_log(1, "%s: fwrite: %s: %s",
                        fid, chn->cmpfile, syserrmsg(errno)
                    );
                    return -14;
                }
            }

    /* write the checksum */

            util_log(2, "write checksum to %s", chn->outfile);

            sprintf(gsebuf, "CHK2 %ld", chn->gse.checksum);
            if ((util_lenprt(out, gsebuf, linlen, '\\')) != 0) {
                util_log(1, "%s: util_lenprt: %s: %s",
                    fid, chn->cmpfile, syserrmsg(errno)
                );
                return -18;
            }

    /* delete intermediate files */

            util_log(2, "close files and delete %s and %s",
                chn->rawfile, chn->cmpfile
            );

            fclose(chn->raw);
            fclose(cmp);
            fclose(out);
            if (unlink(chn->rawfile) != 0) {
                util_log(1, "%s: warning: can't unlink `%s': %s",
                    fid, chn->rawfile, syserrmsg(errno)
                );
            }
            if (unlink(chn->cmpfile) != 0) {
                util_log(1, "%s: warning: can't unlink `%s': %s",
                    fid, chn->cmpfile, syserrmsg(errno)
                );
            }
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: togse.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
