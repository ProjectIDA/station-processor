#pragma ident "$Id: paro.h,v 1.7 2009/05/14 16:27:56 dechavez Exp $"
/*
 * Paroscientific Digiquartz barometer support
 */

#ifndef paro_h_defined
#define paro_h_defined

#include "platform.h"
#include "ttyio.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#define PARO_BUFLEN  64
#define PARO_MAXCHAR 16
#define PARO_TO       5
#define PARO_MAXTRY   5

/* Flags (DO NOT CHANGE ORDER OR VALUE!) */

#define PARO_MINKEY PARO_P1
#define PARO_P1   1
#define PARO_P2   2
#define PARO_P3   3
#define PARO_P4   4
#define PARO_P5   5
#define PARO_P6   6
#define PARO_P7   7
#define PARO_Q1   8
#define PARO_Q2   9
#define PARO_Q3  10
#define PARO_Q4  11
#define PARO_Q5  12
#define PARO_Q6  13
#define PARO_DB  14
#define PARO_DS  15
#define PARO_BL  16
#define PARO_EW  17
#define PARO_PR  18
#define PARO_TR  19
#define PARO_UN  20
#define PARO_UF  21
#define PARO_MD  22
#define PARO_VR  23
#define PARO_MC  24
#define PARO_CS  25
#define PARO_CT  26
#define PARO_CX  27
#define PARO_SN  28
#define PARO_PA  29
#define PARO_PM  30
#define PARO_TC  31
#define PARO_C1  32
#define PARO_C2  33
#define PARO_C3  34
#define PARO_D1  35
#define PARO_D2  36
#define PARO_T1  37
#define PARO_T2  38
#define PARO_T3  39
#define PARO_T4  40
#define PARO_T5  41
#define PARO_U0  42
#define PARO_Y1  43
#define PARO_Y2  44
#define PARO_Y3  45
#define PARO_XM  46
#define PARO_IA  47
#define PARO_XT  48
#define PARO_XN  49
#define PARO_PI  50
#define PARO_TI  51
#define PARO_BR  52
#define PARO_MAXKEY PARO_BR

#define PARO_INT    1
#define PARO_DOUBLE 2
#define PARO_STRING 3

/* Structure templates */

typedef struct {
    char   sn[PARO_MAXCHAR+1]; /* serial number*/
    char   vr[PARO_MAXCHAR+1]; /* firmware version */
    int    un;   /* units flag */
    double uf;   /* units factor */
    int    pr;   /* pressure resolution */
    int    tr;   /* temperature resolution */
    int    pi;   /* pressure integration time */
    int    ti;   /* temperature integration time */
    int    xm;   /* nano-resolution mode */
    int    ia;   /* IIR cutoff freq parameter */
    int    xt;   /* FIR temperature smoothing */
    int    xn;   /* number of significant digits */
    double pa;   /* additive factor */
    double pm;   /* multiplicative factor */
    double tc;   /* timebase correction factor */
    double c1;   /* C1 pressure coefficient */
    double c2;   /* C2 pressure coefficient */
    double c3;   /* C3 pressure coefficient */
    double d1;   /* D1 pressure coefficient */
    double d2;   /* D2 pressure coefficient */
    double t1;   /* T1 pressure coefficient */
    double t2;   /* T2 pressure coefficient */
    double t3;   /* T3 pressure coefficient */
    double t4;   /* T4 pressure coefficient */
    double t5;   /* T5 pressure coefficient */
    double u0;   /* U0 pressure coefficient */
    double y1;   /* Y1 pressure coefficient */
    double y2;   /* Y2 pressure coefficient */
    double y3;   /* Y3 pressure coefficient */
    double revision;
} PARO_PARAM;

typedef struct paro_info {
    TTYIO *tty;
    LOGIO *lp;
    char buf[PARO_BUFLEN];
    BOOL debug;
    PARO_PARAM param;
} PARO;

/* contants for default 1 sps operation */

#define PARO_MIN_PI_REVISION ((double) 5.1) // not really, waiting for Paro to reply about when the actual change was

#define PARO_DEFAULT_PR 119
#define PARO_DEFAULT_PI 333
#define PARO_DEFAULT_UN 0
#define PARO_DEFAULT_UF 68947.57
#define PARO_DEFAULT_PA 0
#define PARO_DEFAULT_PM 1

/* constants for nano enabled systems */

#define PARO_MIN_XM_REVISION ((double) 5.1)

#define PARO_DEFAULT_XM 1
#define PARO_DEFAULT_IA 11
#define PARO_DEFAULT_XN 10

/* for saving nano precision readings as INT32 */

#define PARO_NORMAL_VALUE  1000000
#define PARO_NORMAL_FACTOR   10000

/* Function prototypes */

/* paro.c */
PARO *paroOpen(char *port, int speed, LOGIO *lp, BOOL debug);
char *paroCommand(PARO *pp, int key);
char *paroItoa(int key);
char *paroRawval(PARO *pp, int key, int *type);
char *paroRead(PARO *pp);
BOOL paroReadINT32(PARO *pp, INT32 *result);
int   paroAtoi(char *string);
int   paroGetcnf(PARO *pp);
int   paroGetval(PARO *pp, int key, void *ptr);
int   paroReset(PARO *pp);
int   paroSet(PARO *pp, int key, char *value);
BOOL  paroWrite(PARO *pp, char *string);
void  paroClose(PARO *pp);
BOOL  paroVerify(PARO *pp, PARO_PARAM *want);
BOOL  paroInit(PARO *pp, PARO_PARAM *want);

/* version.c */
char *paroVersionString(void);
VERSION *paroVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* paro_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: paro.h,v $
 * Revision 1.7  2009/05/14 16:27:56  dechavez
 * added PARO_BR
 *
 * Revision 1.6  2009/03/24 20:54:50  dechavez
 * updated paroOpen() prototype, removed verbose flag from PARO_PARAM and changed
 * it in PARO handle to debug
 *
 * Revision 1.5  2009/03/20 21:19:26  dechavez
 * updated prototypes, added pi, ti, xm, ia, xt, xn, revision fields, added
 * PARO_PARAM to PARO handle
 *
 * Revision 1.4  2006/10/17 19:11:22  dechavez
 * increased buffer size (not really needed) and added verbose fields to
 * the PARO and PARO_PARAM structures
 *
 * Revision 1.3  2006/05/17 23:21:24  dechavez
 * added copyright notice
 *
 * Revision 1.2  2006/03/30 22:45:56  dechavez
 * initial 1.0.0 release
 *
 */
