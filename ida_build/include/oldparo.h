#pragma ident "$Id: oldparo.h,v 1.1 2006/03/30 22:45:31 dechavez Exp $"
/*
 * Paroscientific Digiquartz barometer support (deprecated version)
 */

#ifndef oldparo_h_defined
#define oldparo_h_defined

/* Constants */

#define PARO_BUFLEN  32
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
#define PARO_MAXKEY PARO_Y3

#define PARO_INT    1
#define PARO_DOUBLE 2
#define PARO_STRING 3

/* Structure templates */

struct paro_info {
    int fd;
    int to;
    char buf[PARO_BUFLEN];
};

typedef struct paro_info PARO;

struct paro_param {
    char   sn[PARO_MAXCHAR+1]; /* serial number    */
    char   vr[PARO_MAXCHAR+1]; /* firmware version */
    int    un;   /* units flag                     */
    double uf;   /* units factor                   */
    int    pr;   /* pressure resolution            */
    int    tr;   /* temperature resolutin          */
    double pa;   /* additive factor                */
    double pm;   /* multiplicative factor          */
    double tc;   /* timebase correction factor     */
    double c1;   /* C1 pressure coefficient        */
    double c2;   /* C2 pressure coefficient        */
    double c3;   /* C3 pressure coefficient        */
    double d1;   /* D1 pressure coefficient        */
    double d2;   /* D2 pressure coefficient        */
    double t1;   /* T1 pressure coefficient        */
    double t2;   /* T2 pressure coefficient        */
    double t3;   /* T3 pressure coefficient        */
    double t4;   /* T4 pressure coefficient        */
    double t5;   /* T5 pressure coefficient        */
    double u0;   /* U0 pressure coefficient        */
    double y1;   /* Y1 pressure coefficient        */
    double y2;   /* Y2 pressure coefficient        */
    double y3;   /* Y3 pressure coefficient        */
};

/* Function prototypes */

PARO *paro_open(char *port, int speed, struct paro_param *param);
char *paro_command(PARO *pp, int key);
char *paro_itoa(int key);
char *paro_rawval(PARO *pp, int key, int *type);
char *paro_read(PARO *pp);
int   paro_atoi(char *string);
int   paro_getcnf(PARO *pp, struct paro_param *param);
int   paro_getval(PARO *pp, int key, void *ptr);
int   paro_reset(PARO *pp);
int   paro_set(PARO *pp, int key, char *value);
int   paro_test(PARO *pp);
int   paro_write(PARO *pp, char *string);
void  paro_close(PARO *pp);

#endif

/* Revision History
 *
 * $Log: oldparo.h,v $
 * Revision 1.1  2006/03/30 22:45:31  dechavez
 * former paro.h
 *
 */
