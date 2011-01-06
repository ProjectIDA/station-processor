#pragma ident "$Id: db.h,v 1.8 2007/01/11 22:00:22 dechavez Exp $"
/*======================================================================
 *
 *  ISI database support
 *
 *====================================================================*/
#ifndef isi_db_h_included
#define isi_db_h_included

#include "platform.h"
#include "dbio.h"
#include "ida.h"
#include "isi.h"
#include "isi/dl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Prototyypes */

/* calib.c */
BOOL isidbLookupCalib(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, double *calib, double *calper);

/* chan.c */
LNKLST *isidbCalibrationChanList(DBIO *db, char *sta, UINT32 when);

/* chnlocmap.c */
LNKLST *isidbChnLocMapList(DBIO *db, char *name);

/* coords.c */
BOOL isidbLookupCoords(DBIO *db, char *site, UINT32 when, ISI_COORDS *coords);

/* datlen.c */
BOOL isidbLookupDatlen(DBIO *db, ISI_STREAM_NAME *name, int *datlen);

/* debug.c */
BOOL isidbLookupDebug(DBIO *db, char *app, ISI_DEBUG *debug);

/* inst.c */
BOOL isidbLookupInst(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, ISI_INST *inst);

/* revs.c */
BOOL isidbLookupRev(DBIO *db, int value, IDA_REV *result);
LNKLST *isidbRevList(DBIO *db);

/* root.c */
BOOL isidbLookupRoot(DBIO *db, char *app, char *root);

/* sint.c */
BOOL isidbLookupSint(DBIO *db, ISI_STREAM_NAME *name, int *sint);

/* site.c */
LNKLST *isidbSiteList(DBIO *db);

/* sitechan.c */
BOOL isidbLookupSitechan(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, ISI_INST *inst);
BOOL isidbGenerateIsiSitechanTable(DBIO *db);

/* sitechanflag.c */
  BOOL isidbLookupSitechanflag(DBIO *db, char *name, char *chan, ISI_SITECHANFLAG *dest);

/* srate.c */
BOOL isidbLookupSrate(DBIO *db, ISI_STREAM_NAME *name, ISI_SRATE *sint);

/* stamap.c */
char *isidbLookupStamap(DBIO *db, char *sta, UINT32 when, char *dest, int len);

/* system.c */
LNKLST *isidbSystemList(DBIO *db);
BOOL isidbLookupSystemByName(DBIO *db, char *name, ISI_SYSTEM *dest);
BOOL isidbLookupSysFlagsByName(DBIO *db, char *name, ISI_SYS_FLAGS *dest);
static BOOL MysqlSystemLookup(DBIO *db, char *name, ISI_SYSTEM *dest);
static BOOL FlatfileSystemLookup(DBIO *db, char *name, ISI_SYSTEM *dest);

/* tqual.c */
BOOL isidbLookupTqual(DBIO *db, char factory, int *internal);
LNKLST *isidbTqualList(DBIO *db);

/* trecs.c */
BOOL isidbLookupTrecs(DBIO *db, UINT32 *trecs);

/* wrdsiz.c */
BOOL isidbLookupWrdsiz(DBIO *db, ISI_STREAM_NAME *name, int *wrdsiz);

#ifdef __cplusplus
}
#endif

#endif /* isi_db_h_included */

/* Revision History
 *
 * $Log: db.h,v $
 * Revision 1.8  2007/01/11 22:00:22  dechavez
 * renamed all functions to use isidb prefix
 *
 * Revision 1.7  2007/01/04 21:47:23  judy
 * updated prototypes
 *
 * Revision 1.6  2006/11/10 06:49:27  judy
 * updated prototypes
 *
 * Revision 1.5  2006/08/30 18:09:04  judy
 * removed ISI_SYSTEM and ISI_SYS_FLAGS (moved to isi.h)
 *
 * Revision 1.4  2006/08/30 17:48:37  dechavez
 * added ISI_SYSTEM (and ISI_SYS_FLAGS) definitions
 *
 * Revision 1.3  2006/03/13 22:13:03  dechavez
 * updated prototypes
 *
 * Revision 1.2  2006/02/14 17:05:05  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/09 20:08:42  dechavez
 * initial release
 *
 */
