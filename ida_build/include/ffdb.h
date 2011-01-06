#pragma ident "$Id: ffdb.h,v 1.5 2006/11/10 06:25:27 dechavez Exp $"
/*======================================================================
 *
 *  IDA flat file lookups
 *
 *====================================================================*/
#ifndef ida_ff_h_included
#define ida_ff_h_included

#include "platform.h"
#include "ida/limits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int filt;       /* data logger filter number  */
    int chan;       /* data logger channel number */
    int trig;       /* triggered flag             */
    char chname[IDA_CNAMLEN+1]; /* chnloc name    */
} IDAFF_STREAM;

typedef struct {
    char factory;
    int  internal;
} IDAFF_TQUAL;

typedef struct {
    int code;
    char *text;
} IDAFF_REVMAP;

typedef struct {
    char name[IDA_MNAMLEN];
    int nstream;
    IDAFF_STREAM *stream;
} IDAFF_CHANMAP;

typedef struct {
    char sta[IDA_SNAMLEN];
    char chn[IDA_CNAMLEN];
    int sint;
    int datlen;
    int wrdsiz;
} IDAFF_SINTMAP;

typedef struct {
    char sta[IDA_SNAMLEN];
    char name[IDA_MNAMLEN];
} IDAFF_STAMAP;

typedef struct {
    char sta[IDA_SNAMLEN];
    char port[IDA_PORTNAMLEN];
    char device[IDA_DEVNAMLEN];
    INT32 ibaud;
    INT32 obaud;
    int flow;
    int parity;
    int sbits;
} IDAFF_SERIALPORT;

typedef struct {
    int pkt;
    int ttag;
    int bwd;
    int dl;
    int lock;
} IDAFF_DEBUG;

typedef struct {
    char root[MAXPATHLEN+1];
    UINT32 trecs;
    IDAFF_DEBUG debug;
} IDAFF_GLOB;

typedef struct {
    int nmap;
    IDAFF_CHANMAP  *chanmap;
    int nqual;
    IDAFF_TQUAL *tqual;
    int nrev;
    IDAFF_REVMAP *rev;
    int nsint;
    IDAFF_SINTMAP  *sint;
    int nstamap;
    IDAFF_STAMAP *stamap;
    int nserial;
    IDAFF_SERIALPORT  *serial;
    LNKLST systems;
    IDAFF_GLOB glob;
} IDAFF;

/* Function prototypes */

/* ff.c */
void idaffDestroy(IDAFF *ff);
IDAFF *idaffInit(char *spec);
BOOL idaffLookupTqual(IDAFF *ff, char factory, int *internal);
BOOL idaffLookupSintmap(IDAFF *ff, IDAFF_SINTMAP *map);
char *idaffLookupStamap(IDAFF *ff, char *sta);

/* glob.c */
void idaffDefaultGlob(IDAFF_GLOB *glob);
BOOL idaffReadGlobalInitFile(char *path, IDAFF_GLOB *glob);
void idaffLookupGlob(IDAFF *ff, IDAFF_GLOB *glob);

#ifdef __cplusplus
}
#endif

#endif /* ida_ff_h_included */

/* Revision History
 *
 * $Log: ffdb.h,v $
 * Revision 1.5  2006/11/10 06:25:27  dechavez
 * added ttag to debug structure
 *
 * Revision 1.4  2006/06/26 22:18:28  dechavez
 * Change trecs to UINT32
 *
 * Revision 1.3  2006/03/13 22:12:24  dechavez
 * defined and added IDAFF_GLOB
 *
 * Revision 1.2  2006/02/14 17:05:04  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/08 23:51:41  dechavez
 * initial release
 *
 */
