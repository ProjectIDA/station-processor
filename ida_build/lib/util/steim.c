#pragma ident "$Id: steim.c,v 1.5 2005/05/25 22:41:46 dechavez Exp $"
/*======================================================================
 *
 *  util_csteim1 - Steim 1 compression
 *  Here we force the output to be in big endian byte order, as it looks
 *  like many of the big endian decompressors out there assume that the
 *  data are that way.
 *
 *  util_fcsteim1 - FILE based version of the above
 *
 *  util_dsteim1 - Steim 1 decompressor
 *
 *  util_dsteim2 - Steim 2 decompressor
 *
 *  Both decompressors are meant to handle data from both big-endian and
 *  little endian systems, without the assumption that the data are in
 *  big endian byte order.
 *
 *  NOTE: REQUIRES FOUR BYTE LONGS
 *
 *  MT unsafe!
 *
 *====================================================================*/
#include "util.h"
#include "seed.h" /* for the data type constants */

#define FLEN 64           /* number of bytes per frame     */
#define NSEQ 16           /* number of sequences per frame */
#define WLEN FLEN / NSEQ  /* number of bytes per sequence  */

/* local globals */

long *out;   /* output buffer                                     */
long *frame; /* current frame                                     */
long fi;     /* index of current frame                            */
long maxfrm; /* number of frames that will fit into output buffer */
int  si;     /* sequence index in current frame                   */
long nc;     /* number of compressed samples in output            */
long ocount; /* number of bytes used in output buffer             */
int  swap;   /* swap flag, set if host is not big endian          */

float  float_factor;
double double_factor;

static int ns = 0; /* number of diffs currently saved */

/* internal function: load one compressed word into output */

static int csteim1_addword(long *d, int ndiff)
{
union {
    char  *c;
    short *s;
    long  *l;
} ptr;
static char key[5] = {0, 3, 2, 0, 1};

    if (ndiff == 0) return 0;

/* Load in the differences and associated descriptor */

    ptr.l = frame + si;
    if (ndiff == 4) {
        ptr.c[0]  = (char) (d[0] & 0xff);
        ptr.c[1]  = (char) (d[1] & 0xff);
        ptr.c[2]  = (char) (d[2] & 0xff);
        ptr.c[3]  = (char) (d[3] & 0xff);
    } else if (ndiff == 2) {
        ptr.s[0]  = (short) (d[0] & 0xffff);
        ptr.s[1]  = (short) (d[1] & 0xffff);
        if (swap) util_sswap(ptr.s, 2);
    } else if (ndiff == 1) {
        ptr.l[0]  = d[0];
        if (swap) util_lswap(ptr.l, 1);
    }
    *frame |= (key[ndiff] << (2 * (15 - si)));

    ocount += 4;
    nc += ndiff;

/* Increment the sequence counter */

    if (++si < NSEQ) return 0;

/* At this point the frame has been completed. */

    if (swap) util_lswap(frame, 1);
    if (++fi == maxfrm) return 1; /* No more frames available, done */

/* Initialize new frame */

    frame    = out + (fi * NSEQ);
    *frame   = 0;
    ocount  += 4;
    si       = 1;

    return 0;
}

/* internal function: build a frame */
    
static int csteim1_frm(long diff, int flush)
{
/* Possible states */

#define STATE_NUL  0 /* no differences saved so far      */
#define STATE_1D1  1 /* 1 1-byte difference saved so far */
#define STATE_2D1  2 /* 2 1-byte difference saved so far */
#define STATE_3D1  3 /* 3 1-byte difference saved so far */
#define STATE_1D2  4 /* 1 2-byte difference saved so far */
#define STATE_BAD -1 /* will never occur                 */

struct activity {
    int flush1;   /* no. diffs to flush right away                        */
    int flush2;   /* no. diffs to flush after that and before saving crnt */
    int flush3;   /* no. diffs to flush after saving crnt diff            */
};

static struct activity action_table[5][5] = {
            /*                  size of difference                      */
            /*        0          1          2          3          4     */
/* STATE_NUL */ {  {0,0,0},   {0,0,0},   {0,0,0},   {0,0,0},   {0,0,1} },
/* STATE_1D1 */ {  {1,0,0},   {0,0,0},   {0,0,2},   {0,0,0},   {1,0,1} },
/* STATE_2D1 */ {  {2,0,0},   {0,0,0},   {2,0,0},   {0,0,0},   {2,0,1} },
/* STATE_3D1 */ {  {2,1,0},   {0,0,4},   {2,0,2},   {0,0,0},   {2,1,1} },
/* STATE_1D2 */ {  {1,0,0},   {0,0,2},   {0,0,2},   {0,0,0},   {1,0,1} }
};

static int state_table[5][5] = {
            /*                  size of difference                      */
            /*        0          1          2          3          4     */
/* STATE_NUL */ {STATE_NUL, STATE_1D1, STATE_1D2, STATE_BAD, STATE_NUL},
/* STATE_1D1 */ {STATE_NUL, STATE_2D1, STATE_NUL, STATE_BAD, STATE_NUL},
/* STATE_2D1 */ {STATE_NUL, STATE_3D1, STATE_1D2, STATE_BAD, STATE_NUL},
/* STATE_3D1 */ {STATE_NUL, STATE_NUL, STATE_NUL, STATE_BAD, STATE_NUL},
/* STATE_1D2 */ {STATE_NUL, STATE_NUL, STATE_NUL, STATE_BAD, STATE_NUL}
};

int i, j, size, nf, done;
struct activity action;
static long d[4];
static int state = STATE_NUL;

/* Figure out how many bytes are needed for this difference */

    if (flush) {
        size = 0;
    } else if (diff >= -128 && diff <= 127) {
        size = 1;
    } else if (diff > -32768 && diff <= 32767) {
        size = 2;
    } else {
        size = 4;
    }

/* Figure out what to do with this difference */

    action = action_table[state][size];
    state  =  state_table[state][size]; /* new state, after action */

    if (nf = action.flush1) {
        done = csteim1_addword(d, nf);
        for (i = 0, j = nf; j < ns; i++, j++) d[i] = d[j];
        ns -= nf;
        if (done) return 1;
    }

    if (nf = action.flush2) {
        done = csteim1_addword(d, nf);
        for (i = 0, j = nf; j < ns; i++, j++) d[i] = d[j];
        ns -= nf;
        if (done) return 1;
    }

    if (size) d[ns++] = diff;

    if (nf = action.flush3) {
        done = csteim1_addword(d, nf);
        for (i = 0, j = nf; j < ns; i++, j++) d[i] = d[j];
        ns -= nf;
        if (done) return 1;
    }

    return 0;
}

/* Steim 1 compression */

/* Given a fixed length array to store the output, this routine
 * will compress as many samples as possible into this array,
 * starting at offset 0.  The number of samples which were 
 * compressed (which may be less that the input count) is
 * returned.
 */

/* char *dest   => output array                         */
/* long destlen => size of dest, in bytes               */
/* long *src    => input array                          */
/* long count   => number of entries in src             */
/* long *used   => number of output bytes actually used */

long util_csteim1(char *dest, long destlen, long *src, long count, long *used)
{
long s, diff;

    if (sizeof(long) != WLEN) {
        errno = EINVAL;
        return -1;
    }

/* Initialize everything */

    memset(dest, 0, destlen);

    swap   = (util_order() == LTL_ENDIAN_ORDER);
    out    = (long *) dest;
    out[1] = src[0];
    out[2] = src[0];
    ocount = 12; /* W0, X0, Xn to start */
    maxfrm = destlen / FLEN; /* maximum number of output frames */
    frame  = out;
    *frame = 0;
    fi     = 0;
    nc     = 0;
    si     = 3;

/* Loop for each sample (will bail when we run out of output memory) */

    for (s = 0; s < count; s++) {
        diff = src[s] - out[2];
        if (csteim1_frm(diff, 0) != 0) {
            if (used != (long *) NULL) *used = ocount;
            if (swap) {
                util_lswap(out+1, 2);
            }
            return nc;
        }
        out[2] = src[s];
    }

/* All the input fit into the output.  Flush accumulated differences. */

    csteim1_frm(0, 1);
    if (used != (long *) NULL) *used = ocount;
    if (swap) {
        util_lswap(frame, 1);
        util_lswap(out+1, 2);
    }
    return nc;
}

static int read_short(FILE *fp, long *sample, int *done)
{
short sval;

    if (fread(&sval, sizeof(short), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    *sample = (long) sval;
    return 0;
}

static int read_long(FILE *fp, long *sample, int *done)
{
    if (fread(sample, sizeof(long), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    return 0;
}

static int read_float(FILE *fp, long *sample, int *done)
{
float fval;

    if (fread(&fval, sizeof(float), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    *sample = (long) fval;
    return 0;
}

static int read_double(FILE *fp, long *sample, int *done)
{
double dval;

    if (fread(&dval, sizeof(double), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    *sample = (long) dval;
    return 0;
}

static int read_float_gain(FILE *fp, long *sample, int *done)
{
float fval;

    if (fread(&fval, sizeof(float), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    *sample = (long) (fval * float_factor);
    return 0;
}

static int read_double_gain(FILE *fp, long *sample, int *done)
{
double dval;

    if (fread(&dval, sizeof(double), 1, fp) != 1) {
        *done = 1;
        return -1;
    }
    *sample = (long) (dval * double_factor);
    return 0;
}

/* Given a FILE of data, do the same as the util_csteim1. */

/* char *dest   => output array                                 */
/* long destlen => size of dest, in bytes                       */
/* FILE *fp     => input FILE                                   */
/* int  type    => type of data in FILE                         */
/* double gain  => used when converting from floats and doubles */
/* long *used   => number of output bytes actually used         */
/* int *done    => set when input is used up                    */

long util_fcsteim1(char *dest, long destlen, FILE *fp, int type, double gain, long *used, int *done)
{
long diff, sample;
int (*get_sample)();

    if (sizeof(long) != WLEN) {
        errno = EINVAL;
        return -1;
    }

    if (sizeof(short) != 2) {
        errno = EINVAL;
        return -2;
    }

/* Figure out how to read the input */

    switch (type) {
      case SEED_INT_16:
        get_sample = read_short;
        break;
      case SEED_INT_32:
        get_sample = read_long;
        break;
      case SEED_IEEE_F:
        if (gain == (double) 1.0) {
            get_sample = read_float;
        } else {
            get_sample = read_float_gain;
            float_factor = (float) gain;
        }
        break;
      case SEED_IEEE_D:
        if (gain == (double) 1.0) {
            get_sample = read_double;
        } else {
            get_sample = read_double_gain;
            double_factor = gain;
        }
        break;
      default:
        errno = EINVAL;
        return -3;
    }

/* Initialize everything */

    memset(dest, 0, destlen);

    if ((*get_sample)(fp, &sample, done) != 0) return 0;

    swap   = (util_order() == LTL_ENDIAN_ORDER);
    out    = (long *) dest;
    out[1] = sample;
    out[2] = sample;
    ocount = 12; /* W0, X0, Xn to start */
    maxfrm = destlen / FLEN; /* maximum number of output frames */
    frame  = out;
    *frame = 0;
    fi     = 0;
    nc     = 0;
    si     = 3;

/* Do first sample */

    diff = sample - out[2];
    if (csteim1_frm(diff, 0) != 0) {
        if (used != (long *) NULL) *used = ocount;
        if (swap) {
            util_lswap(out+1, 2);
        }
        return nc;
    }
    out[2] = sample;

/* Loop for each sample (will bail at eof or read error) */

    while ((*get_sample)(fp, &sample, done) == 0) {
        diff = sample - out[2];
        if (csteim1_frm(diff, 0) != 0) {
            if (used != (long *) NULL) *used = ocount;
            if (swap) {
                util_lswap(out+1, 2);
            }
            return nc;
        }
        out[2] = sample;
    }
    if (nc == 0) return 0;

/* All the input fit into the output.  Flush accumulated differences. */

    csteim1_frm(0, 1);
    if (used != (long *) NULL) *used = ocount;
    if (swap) {
        util_lswap(frame, 1);
        util_lswap(out+1, 2);
    }
    return nc;
}

/* Steim 1 decompression */

/* long *dest   => output array                            */
/* long destlen => dimension of output array               */
/* char *src    => input Steim 1 compressed data           */
/* long srclen  => size of src, in bytes                   */
/* u_long order => byte order (as per util_order()) of src */
/* long count   => number of uncompressed samples in src   */

int util_dsteim1(long *dest, long destlen, char *src, long srclen, u_long order, long count)
{
int i, j, k, nsamp, nfrm;
long  ltmp, val, beg, end, key, code[NSEQ];
short stmp;
char *frm;
union {
    char  *c;
    short *s;
    long  *l;
} ptr;

    if (sizeof(long) != WLEN) {
        errno = EINVAL;
        return -1;
    }

    swap = (order != util_order());
    nfrm = srclen / FLEN;

/* Get the block start/stop values */

    ptr.c = src;

    memcpy(&beg, ptr.c + 4, 4);
    if (swap) util_lswap(&beg, 1);

    memcpy(&end, ptr.c + 8, 4);
    if (swap) util_lswap(&end, 1);

/* Loop over each frame */
/* We do not verify that the 0x00 codes are where they should be */

    val   = dest[0] = beg;
    nsamp = 1;

    for (i = 0; i < nfrm; i++) {

        frm = src + (i * FLEN);  /* point to start of current frame */
        key = *((long *) frm);   /* codes are in first 4 bytes      */
        if (swap) util_lswap(&key, 1);
        for (j = NSEQ - 1; j >= 0; j--) {
            code[j] = key & 0x03;
            key >>= 2;
        }

        for (j = 1; j < NSEQ; j++) {

            if (nsamp >= destlen) {
                return -2;
            }

            ptr.c = frm + (j * 4);  /* point to current 4 byte sequence */

            switch (code[j]) {
              case 0:
                break;

              case 1:
                for (k = (nsamp == 1) ? 1 : 0; k < 4; k++) {
                    dest[nsamp++] = (val += (long) ptr.c[k]);
                }
                break;

              case 2:
                for (k = (nsamp == 1) ? 1 : 0; k < 2; k++) {
                    stmp = ptr.s[k];
                    if (swap) util_sswap(&stmp, 1);
                    dest[nsamp++] = (val += (long) stmp);
                }
                break;

              case 3:
                if (nsamp > 1) {
                    ltmp = ptr.l[0];
                    if (swap) util_lswap(&ltmp, 1);
                    dest[nsamp++] = (val += ltmp);
                }
                break;

              default:
                return -3;
            }
        }
    }

/* Sanity checks */

#ifdef INCLUDE_SANITY_CHECKS
    if (count != nsamp) return -4;
    if (dest[nsamp-1] != end) return -5;
#endif /* INCLUDE_SANITY_CHECKS */

    return 0;

}

static int do_steim2(
int code,      /* key code                         */
long word,     /* 4-byte sequence to decode        */
long *output,  /* ptr to output array              */
long nsamp,    /* current sample count             */
long maxsamp,  /* maximum number of words to store */
int swap       /* byte swap flag                   */
){
long dnib, rest, shift, diff;
struct decode_info {
    long ndif;
    long bits;
} decode;
static struct decode_info decode_table[4][4] = {
            /*                    dnib                  */
            /*       0         1         2         3    */
/* code = 0 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = 1 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = 2 */ { {-1, -1}, { 1, 30}, { 2, 15}, { 3, 10} },
/* code = 3 */ { { 5,  6}, { 6,  5}, { 7,  4}, {-1, -1} }
};
struct mask_info {
    long sign;
    long diff;
} mask;
static struct mask_info mask_table[4][4] = {
/* code = 0 */ { {         0,          0}, {         0,          0},
                 {         0,          0}, {         0,          0} },

/* code = 1 */ { {         0,          0}, {         0,          0},
                 {         0,          0}, {         0,          0} },

/* code = 2 */ { {         0,          0}, {0x20000000, 0x3fffffff},
                 {0x00004000, 0x00007fff}, {0x00000200, 0x000003ff} },

/* code = 3 */ { {0x00000020, 0x0000003f}, {0x00000010, 0x0000001f},
                 {0x00000008, 0x0000000f}, {         0,          0} }
};

    if (code == 0) return nsamp;
    if (swap) util_lswap(&word, 1);
    dnib = (word >> 30) & 0x03;
    rest =  word & 0x3fffffff;

    if (code < 0 || code > 3) return -1;
    if (dnib < 0 || dnib > 3) return -2;

    decode = decode_table[code][dnib];
    if (decode.ndif < 0 || decode.bits < 0) return -3;

    mask = mask_table[code][dnib];

    for (
        shift = decode.bits * (decode.ndif - 1);
        shift >= 0 && nsamp < maxsamp;
        shift -= decode.bits
    ) {
        diff = (rest >> shift) & mask.diff;
        if (diff & mask.sign) diff |= ~mask.diff;
        output[nsamp++] = diff;
    }
    return nsamp;
}

/* Steim 2 decompression */

/* long *dest   => output array                            */
/* long destlen => dimension of output array               */
/* char *src    => input Steim 2 compressed data           */
/* long srclen  => size of src, in bytes                   */
/* u_long order => byte order (as per util_order()) of src */
/* long count   => number of uncompressed samples in src   */

int util_dsteim2(long *dest, long destlen, char *src, long srclen, u_long order, long count)
{
int i, j, k, nsamp, nfrm;
long  beg, end, key, code[NSEQ];
char *frm;
union {
    char  *c;
    short *s;
    long  *l;
} ptr;

    if (sizeof(long) != WLEN) {
        errno = EINVAL;
        return -1;
    }

    swap = (order != util_order());
    nfrm = srclen / FLEN;

/* Get the block start/stop values */

    ptr.c = src;

    memcpy(&beg, ptr.c + 4, 4);
    if (swap) util_lswap(&beg, 1);

    memcpy(&end, ptr.c + 8, 4);
    if (swap) util_lswap(&end, 1);

/* Loop over each frame */
/* We do not verify that the 0x00 codes are where they should be */

    for (nsamp = 0, i = 0; i < nfrm; i++) {

        frm = src + (i * FLEN);  /* point to start of current frame */
        key = *((long *) frm);   /* codes are in first 4 bytes      */
        if (swap) util_lswap(&key, 1);
        for (j = NSEQ - 1; j >= 0; j--) {
            code[j] = key & 0x03;
            key >>= 2;
        }

        for (j = 0; j < NSEQ; j++) {

            if (nsamp >= destlen) return -2;

            ptr.c = frm + (j * 4);  /* point to current 4 byte sequence */

            if (code[j] == 1) {
                for (k = 0; k < 4; k++) {
                    dest[nsamp++] = (long) ptr.c[k];
                }
            } else if (code[j] != 0) {
                nsamp = do_steim2(code[j],*ptr.l,dest,nsamp,destlen,swap);
                if (nsamp < 0) return -3;
            }
        }
    }

    for (dest[0]=beg, i=1, j = 0; i < nsamp; i++, j++) dest[i] += dest[j];

/* Sanity checks */

    if (count != nsamp) return -4;

    if (dest[nsamp-1] != end) return -5;

    return 0;

}

#ifdef DEBUG_STEIM1

#define NSAMP 17
#define OUTLEN 64
#define NEWLEN 512 /* way bigger than required! */

main()
{
int i;
static long raw[NSAMP] =
{306, 306, 301, 298, 297, 1024, 3000, 3100, 2000, 0, -100, -200, 0, 15, 80111, 80111, -80123};

u_long order;
int retval;
long nc;
char output[OUTLEN];
long new[NEWLEN], used;

    order = BIG_ENDIAN_ORDER;
        
    printf("beg data = ");
    for (i = 0; i < NSAMP; i++) printf("%3ld ", raw[i]); printf("\n");

    nc = util_csteim1(output, OUTLEN, raw, NSAMP, &used);
    printf("util_csteim1 returns %ld (%ld bytes used)\n", nc, used);
    printf("\n");
    util_hexdmp((unsigned char *) output, OUTLEN, 0, 'd');
    printf("\n");

    retval = util_dsteim1(new, NEWLEN, output, OUTLEN, order, NSAMP);
    printf("util_dsteim1 returns %d\n", retval);

    printf("beg data = ");
    for (i = 0; i < NSAMP; i++) printf("%3ld ", raw[i]); printf("\n");
    printf("end data = ");
    for (i = 0; i < NSAMP; i++) printf("%3ld ", new[i]); printf("\n");
}
#endif /* DEBUG_STEIM1 */

#ifdef DEBUG_STEIM2

#include <stdio.h>
#include <math.h>
#include "seed.h"

#define MAXSAMP 8192

main(int argc, char **argv)
{
struct seed_fsdh fsdh;
struct seed_b1000 b1000;
struct seed_b1001 b1001;
int i, status, nread, type, count, srclen = -1;
u_long order;
char *ptr, *string, src[4096];
long next, dest[MAXSAMP];

    nread = fread(src, 1, 4096, stdin);
    if (nread < 48) {
        fprintf(stderr, "nread = %d is too small!\n", nread);
        exit(1);
    }

       if (seed_fsdh(&fsdh, src) != 0) {
           fprintf(stderr, "seed_fsdh failed\n");
           exit(1);
       }

    printf("FSDH\n");
    printf("seqno  = %ld\n", fsdh.seqno);
    printf("staid  = %s\n",  fsdh.staid);
    printf("locid  = %s\n",  fsdh.locid);
    printf("chnid  = %s\n",  fsdh.chnid);
    printf("netid  = %s\n",  fsdh.netid);
    printf("start  = %s\n",  util_dttostr(fsdh.start, 0));
    printf("nsamp  = %hd\n", fsdh.nsamp);
    printf("srfact = %hd\n", fsdh.srfact);
    printf("srmult = %hd\n", fsdh.srmult);
    printf("active = %d\n",  (int) fsdh.active);
    printf("ioclck = %d\n",  (int) fsdh.ioclck);
    printf("qual   = %d\n",  (int) fsdh.qual);
    printf("more   = %d\n",  (int) fsdh.more);
    printf("tcorr  = %ld\n", fsdh.tcorr);
    printf("bod    = %hd\n", fsdh.bod);
    printf("first  = %hd\n", fsdh.first);
    printf("order  = %s\n",  fsdh.order == LTL_ENDIAN_ORDER ? "little endian" : "big endian");
    printf("swap   = %d\n",  fsdh.swap);

    next = fsdh.first;
    do {
        ptr = src + next;
        type = seed_type(ptr, &next, fsdh.swap);
        printf("\nB%d\n", type);
        if (type == 1000) {
            if (seed_b1000(&b1000, ptr) != 0) {
                fprintf(stderr, "seed_b1000 failed\n");
            } else {
                printf("next   = %hd ", b1000.next);
                if (next != b1000.next) {
                    printf("ERROR: next offset confusion!\n");
                } else {
                    printf("\n");
                }
                printf("format = %d ", (int) b1000.format);
                switch (b1000.format) {
                  case  1: string = "16 bit integers"; break;
                  case  2: string = "24 bit integers"; break;
                  case  3: string = "32 bit integers"; break;
                  case  4: string = "IEEE float"; break;
                  case  5: string = "IEEE double"; break;
                  case 10: string = "Steim 1 compressed"; break;
                  case 11: string = "Steim 2 compressed"; break;
                  case 12: string = "GEOSCOPE 1"; break;
                  case 13: string = "GEOSCOPE 1"; break;
                  case 14: string = "GEOSCOPE 1"; break;
                  case 15: string = "USNSN compressed"; break;
                  case 16: string = "CDSN 16 bit gain ranged"; break;
                  case 17: string = "Graefenberg 16 bit gain ranged"; break;
                  case 18: string = "IPG-Strasbourg 16 bit gain ranged"; break;
                  case 30: string = "SRO format"; break;
                  case 31: string = "HGLP format"; break;
                  case 32: string = "DWWSSN gain ranged"; break;
                  case 33: string = "RSTN 16 bit gain ranged"; break;
                  default: string = "unknown";
                }
                printf("(%s)\n", string);
                printf("order  = %d ", (int) b1000.order);
                printf("(%s)\n", b1000.order ? "big endian" : "little endian");
                printf("length = %d ", (int) b1000.length);
                printf("(%d)\n", (int) pow(2.0, (double) b1000.length));
                srclen = (int) (pow(2.0, (double) b1000.length)) - fsdh.bod;
                printf("srclen set to %d\n", srclen);
            }
        } else if (type == 1001) {
            if (seed_b1001(&b1001, ptr) != 0) {
                fprintf(stderr, "seed_b1001 failed\n");
            } else {
                printf("next   = %hd ", b1001.next);
                if (next != b1001.next) {
                    printf("ERROR: next offset confusion!\n");
                } else {
                    printf("\n");
                }
                printf("tqual  = %d\n", (int) b1001.tqual);
                printf("usec   = %d\n", (int) b1001.usec);
                printf("fcount = %d\n", (int) b1001.fcount);
                srclen = b1001.fcount * 64;
                printf("srclen set to %d\n", srclen);
            }
        } else {
            printf("blockette ignored\n");
        }
    } while (next != 0);

    if (srclen < 0) {
        printf("unknown srclen\n");
        exit(1);
    }

    ptr = src + fsdh.bod;
    order = b1000.order ? BIG_ENDIAN_ORDER : LTL_ENDIAN_ORDER;
    status = util_dsteim2(dest, MAXSAMP, ptr, srclen, order, fsdh.nsamp);
    if (status != 0) {
        printf("util_dsteim2 fails with status %d\n", status);
        if (status != -5) exit(1);
    }

    for (i = 0; i < fsdh.nsamp; i++) {
        if (i % 8 == 0) printf("\n");
        printf("%8ld ", dest[i]);
    }
    printf("\n");
    exit(0);
}

#endif /* DEBUG_STEIM2 */

/* Revision History
 *
 * $Log: steim.c,v $
 * Revision 1.5  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.4  2004/07/26 23:06:13  dechavez
 * removed uneccesary includes
 *
 * Revision 1.3  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.2  2000/10/06 19:58:02  dec
 * don't do sanity checks
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
