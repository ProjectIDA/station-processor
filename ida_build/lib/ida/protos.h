#pragma ident "$Id: protos.h,v 1.3 2006/06/26 22:41:52 dechavez Exp $"
/*======================================================================
 *
 *  Prototypes for revision specific functions.
 *
 *====================================================================*/
#ifndef ida_protos_included
#define ida_protos_included

#include "ida.h"

/* cnfrec5.c */
int cnfrec_rev5(IDA *ida, IDA_CONFIG_REC *dest, UINT8 *src);

/* crec5.c */
int crec_rev5(IDA *ida, IDA_CALIB_REC *dest, UINT8 *src);

/* dhead1.c */
int dhead_rev1(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead2.c */
int dhead_rev2(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead3.c */
int dhead_rev3(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead4.c */
int dhead_rev4(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead5.c */
int dhead_rev5(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead6.c */
int dhead_rev6(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead7.c */
int dhead_rev7(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead8.c */
int dhead_rev8(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* dhead9.c */
int dhead_rev9(IDA *db, IDA_DHDR *dest, UINT8 *src);

/* drec1.c */
int drec_rev1(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec2.c */
int drec_rev2(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec3.c */
int drec_rev3(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec4.c */
int drec_rev4(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec5.c */
int drec_rev5(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec6.c */
int drec_rev6(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec7.c */
int drec_rev7(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec8.c */
int drec_rev8(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* drec9.c */
int drec_rev9(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab);

/* evtrec5.c */
int evtrec_rev5(IDA *ida, IDA_EVENT_REC *dest, UINT8 *src);

/* idrec5.c */
int idrec_rev5(IDA *ida, IDA_IDENT_REC *dest, UINT8 *src);

/* logtrec5.c */
int logrec_rev5(IDA *ida, IDA_LOG_REC *dest, UINT8 *src);

/* srec8.c */
int srec_rev8(IDA *unused, IDA_DAS_STATS *dest, UINT8 *src);

#endif /* ida_protos_included */

/* Revision History
 *
 * $Log: protos.h,v $
 * Revision 1.3  2006/06/26 22:41:52  dechavez
 * fixed prototypes
 *
 * Revision 1.2  2006/02/08 23:07:25  dechavez
 * synced to new function names
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
