#pragma ident "$Id: tags.h,v 1.1.1.1 2000/02/08 20:20:04 dec Exp $"
/*======================================================================
 *
 *  Definition of sorting tags.
 *
 *====================================================================*/
#ifndef tags_h_defined
#define tags_h_defined

#define tag(a) if (print & P_TAG) printf("%7.7ld",a); else printf("",a)

#define BASE_TAG    1000L
#define LABEL_TAG   BASE_TAG +  10000L
#define ARCEOF_TAG  BASE_TAG +  20000L
#define SUM_TAG     BASE_TAG +  30000L
#define DMPREC_TAG  BASE_TAG +  40000L
#define IDENT_TAG   BASE_TAG +  50000L
#define CONFIG_TAG  BASE_TAG +  60000L
#define DASSTAT_TAG BASE_TAG +  65000L
#define DASLOG_TAG  BASE_TAG +  70000L
#define ARSLOG_TAG  BASE_TAG +  80000L
#define BADLOG_TAG  BASE_TAG +  90000L
#define EVENT_TAG   BASE_TAG + 100000L
#define CALIB_TAG   BASE_TAG + 110000L
#define DATA_TAG    BASE_TAG + 120000L
#define TTRIP_TAG   BASE_TAG + 130000L
#define UREC_TAG    BASE_TAG + 140000L
#define DUNMAP_TAG  BASE_TAG + 150000L
#define TQUAL_TAG   BASE_TAG + 160000L
#define EXTJMP_TAG  BASE_TAG + 170000L
#define BADTAG_TAG  BASE_TAG + 180000L
#define BADD_TAG    BASE_TAG + 190000L
#define DUPADJ_TAG  BASE_TAG + 200000L
#define DUPDAT_TAG  BASE_TAG + 210000L
#define GERRS_TAG   BASE_TAG + 220000L
#define LASTW_TAG   BASE_TAG + 230000L
#define CORRUPT_TAG BASE_TAG + 240000L
#define SINTERR_TAG BASE_TAG + 250000L
#define PINCERR_TAG BASE_TAG + 260000L
#define SINCERR_TAG BASE_TAG + 270000L
#define NINCERR_TAG BASE_TAG + 280000L
#define LINCERR_TAG BASE_TAG + 290000L
#define EINCERR_TAG BASE_TAG + 300000L
#define TOFFERR_TAG BASE_TAG + 310000L
#define LAST_TAG    BASE_TAG + 320000L

#endif

/* Revision History
 *
 * $Log: tags.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
