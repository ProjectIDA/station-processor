#pragma ident "$Id: service.h,v 1.4 2004/04/26 20:49:36 dechavez Exp $"
#ifndef _drm_service_h_included
#define _drm_service_h_included

#include "drm.h"
#include "util.h"
#include "xfer.h"

/*  Macros  */

#define clear_timer() set_timer((long) 0)

/*  Function prototypes  */

BOOL AsciiOutputFormat(int format);

int drm_anonftp(
    struct drm_datreq *
);

void catch_alarm(
    int
);

void catch_signal(
    int
);

int canned_autodrm(
    struct drm_cnf *,
    struct drm_datreq *
);

int canned_idadrm(
    struct drm_cnf *,
    struct drm_datreq *
);

void convert(
    char *,
    struct drm_datreq *
);

int deliver(
    struct drm_cnf *,
    struct drm_datreq *
);

void die(
    int
);

int drm_email(
    struct drm_cnf *,
    struct drm_datreq *
);

void drm_exit(
    int
);

int drm_ftp(
    struct drm_datreq *,
    int
);

struct drm_info *getinfo(
    char *,
    char *
);

struct xfer_cnfnrts *get_config(
    char *,
    int,
    int,
    int *
);

int gse_msg_init(
    char *,
    struct drm_finfo *,
    char *
);

void help(
    char *
);

void init(
    int,
    char **,
    char **,
    int *,
    char *
);

int merge(
    struct drm_datreq *
);

int prep_line(
    char *,
    char **,
    int,
    char,
    char *
);

int  prepsets(
    struct drm_rawreq *,
    char *
);

int drm_rcp(
    struct drm_datreq *
);

int  service(
    struct drm_datreq *,
    struct drm_cnf *,
    int,
    char *
);

void set_datreq(
    struct drm_datreq *
);

void set_return_address(
    char *
);

void set_timer(
    long
);

int split_req(
    struct drm_datreq *,
    struct drm_datreq **,
    char *
);

int xferdata(
    char *,
    char *,
    struct drm_datreq *,
    struct drm_sta *,
    int
);

int isidata(
    char *,
    char *,
    struct drm_datreq *,
    struct drm_sta *,
    int
);

void remap_stachn(
    struct remap *,
    struct xfer_packet *
);

void load_stachnmap(
    struct remap *
);

int GzipRawFile(char *input, char *output);

#endif /* _drm_service_h_included */

/* Revision History
 *
 * $Log: service.h,v $
 * Revision 1.4  2004/04/26 20:49:36  dechavez
 * update prototypes
 *
 * Revision 1.3  2004/01/23 22:20:43  dechavez
 * updated prototypes
 *
 * Revision 1.2  2003/12/10 06:30:32  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
