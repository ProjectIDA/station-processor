#pragma ident "$Id: rdautodrm.c,v 1.8 2007/11/05 20:19:16 dechavez Exp $"
/*======================================================================
 *
 *  Read a AUTODRM format request.
 *
 *  Note: Currently *only* WAVEFORM requests are supported here
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "drm.h"
#include "service.h"
#include "seed.h"
#include "fnmatch.h"

#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD 0x10
#endif

#define MAXTOKENS 32

#define SYNTAX_ERROR 1
#define DATA_ERROR   2

#define WAVEFORM_REQ 0x01
#define STATION_REQ  0x02
#define CHANNEL_REQ  0x04
#define RESPONSE_REQ 0x08
#define HELP_REQ     0x10

static FILE *efp           = NULL;
static char *default_email = NULL;
static char *return_path   = NULL;
static char *reply_to      = NULL;
static char *from          = NULL;
static int nset            = 0;
static int errors          = 0;
static int have_begin      = 0;
static int have_email      = 0;
static int have_time       = 0;
static int want_help       = 0;
static int want_slist      = 0;
static int help_not_sent   = 1;

static int isSpam(char *line)
{
int i;
static char *target[] = {
    "X-Spam-Flag: Spam YES",
    "X-Spam-Level: Level **",
    "Content-Type: multipart",
    "Content-Transfer-Encoding: base64",
    "@sky.seed.net.tw",
    "html",
    "HTML",
    "HTTP",
    "http",
    "IMG SRC",
    "=?",
    "?=",
    NULL
}; 

    for (i = 0; target[i] != NULL; i++) {
        if (strstr(line, target[i]) != 0) return 1;
    }

    return 0;
}

char *default_address()
{
    if (reply_to    != NULL) return reply_to;
    if (return_path != NULL) return return_path;
    if (from        != NULL) return from;

    return NULL;
}

/* A single AutoDRM request (equal to nsta IDADRM requests) */

struct autodrm_request {
    char *email;
    char *ftp;
    char *sta[DRM_MAXSTA];
    char *chn[DRM_MAXCHN];
    int nsta;
    int nchn;
    double beg;
    double end;
    char *msg_id;
    char *src_id;
    int linelen;
    int format;
    int type;
    int errors;
};

static struct request_list {
    struct autodrm_request *req;
    struct request_list *next;
} head = {NULL, NULL};

static int reject = 0;

static int str_compare(a, b)
char **a;
char **b;
{
    return strcasecmp(*a, *b);
}

struct autodrm_request *new_request()
{
int i;
struct autodrm_request *new;
static char *fid = "new_request";

    new = (struct autodrm_request *) malloc(sizeof(struct autodrm_request));

    if (new == NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

    for (i = 0; i < DRM_MAXSTA; i++) new->sta[i] = NULL;
    for (i = 0; i < DRM_MAXCHN; i++) new->chn[i] = NULL;
    new->email = new->ftp = new->msg_id = new->src_id = NULL;

    new->nsta    = 0;
    new->beg     = (double) -1;
    new->end     = (double) time(NULL);
    new->type    = 0;
    new->errors  = 0;

/* Set defaults */

    new->nchn    = 1;
    new->chn[0]  = DRM_DEFCHNLST;
    new->linelen = DRM_DEFLINLEN;
    new->format  = DRM_CM6;

    return new;
}
    
double cvt_time(dstr, tstr)
char *dstr;
char *tstr;
{
int ntoken;
int yr, mo, da, jd, hr, mn, sc, ms;
float fs;
char *token[MAXTOKENS];
static char *fid = "cvt_time";

    if (dstr == NULL) {
        util_log(1, "%s: fatal error: NULL date string", fid);
        drm_exit(DRM_BAD);
    }
    
    mo = da = 1;
    switch (ntoken = prep_line(dstr, token, MAXTOKENS, 0, "/")) {
      case 1:
        yr = atoi(token[0]);
        break;
      case 2:
        yr = atoi(token[0]);
        mo = atoi(token[1]);
        break;
      case 3:
        yr = atoi(token[0]);
        mo = atoi(token[1]);
        da = atoi(token[2]);
        break;
      default:
        return (double) -1;
    }

    hr = mn = 0; fs = 0.0;
    if (tstr != NULL) {
        switch (ntoken = prep_line(tstr, token, MAXTOKENS, 0, ":")) {
          case 1:
            hr = atoi(token[0]);
            break;
          case 2:
            hr = atoi(token[0]);
            mn = atoi(token[1]);
            break;
          case 3:
            hr = atoi(token[0]);
            mn = atoi(token[1]);
            fs = atof(token[2]);
            break;
          default:
            return (double) -1;
        }
    }

    sc = (int) fs;
    ms = (int) ((fs - (float) sc) * 1000.0);
            
    jd = util_ymdtojd(yr, mo, da);

    return util_ydhmsmtod(yr, jd, hr, mn, sc, ms);
}

int rd_msg_type(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_msg_type";

    if (argc != 2) return SYNTAX_ERROR;
    if (strcasecmp(argv[1], "DATA") == 0) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "data messages neither expected nor allowed\n");
        reject = 1;
    }
    if (strcasecmp(argv[1], "REQUEST") != 0) return DATA_ERROR;
    return 0;
}

int rd_msg_id(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_msg_id";

    if (argc != 3) return SYNTAX_ERROR;
    if ((req->msg_id = strdup(argv[1])) == NULL) {
        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
        return -1;
    }
    if ((req->src_id = strdup(argv[2])) == NULL) {
        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
        return -2;
    }

    return 0;
}

int rd_email(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_email";

    if (req->ftp != NULL) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "FTP command already specified so E-MAIL not permitted\n");
        ++req->errors;
        ++errors;
        return 0;
    } else if (req->email != NULL) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "E-MAIL command already given for this request\n");
        ++req->errors;
        ++errors;
        return 0;
    }

    if (argc != 2) return SYNTAX_ERROR;

    if ((req->email = strdup(argv[1])) == NULL) {
        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
        return -1;
    }

    util_ucase(argv[1]);
    if (
        strstr(argv[1], "AUTODRM") != NULL    ||
        strstr(argv[1], "POSTMASTER") != NULL ||
        strstr(argv[1], "MAILER-DAEMON") != NULL
    ) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "illegal (reserved) email address\n");
        reject = 1;
    }

    have_email = 1;

    return 0;
}

int rd_ftp(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_ftp";

    if (req->email != NULL) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "E-MAIL command already specified so FTP not permitted\n");
        ++req->errors;
        ++errors;
        return 0;
    } else if (req->ftp != NULL) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "FTP command already given for this request\n");
        ++req->errors;
        ++errors;
        return 0;
    }

    if (argc != 2) return SYNTAX_ERROR;

    if ((req->ftp = strdup(argv[1])) == NULL) {
        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
        return -1;
    }

    have_email = 1;

    return 0;
}

int rd_linelen(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_linelen";

    if (argc != 2) return SYNTAX_ERROR;

    req->linelen = atoi(argv[1]);
    if (req->linelen < 2 || req->linelen > 1024) return DATA_ERROR;

    return 0;
}

int rd_time(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
int i, tondx;
double now;
static char *fid = "rd_time";

    now = (double) time(NULL);

    for (tondx = 0, i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "TO") == 0) tondx = (tondx == 0) ? i : -1;
    }

    switch (tondx) {
      case 1:
        req->beg = now;
        switch (argc) {
          case 2: req->end = now; break;
          case 3: req->end = cvt_time(argv[2], NULL); break;
          case 4: req->end = cvt_time(argv[2], argv[3]); break;
          default:
            return SYNTAX_ERROR;
        }
        break;

      case 2:
        req->beg = cvt_time(argv[1], NULL);
        switch (argc) {
          case 3: req->end = now; break;
          case 4: req->end = cvt_time(argv[3], NULL); break;
          case 5: req->end = cvt_time(argv[3], argv[4]); break;
          default:
            return SYNTAX_ERROR;
        }
        break;

      case 3:
        req->beg = cvt_time(argv[1], argv[2]);
        switch (argc) {
          case 4: req->end = now; break;
          case 5: req->end = cvt_time(argv[4], NULL); break;
          case 6: req->end = cvt_time(argv[4], argv[5]); break;
          default:
            return SYNTAX_ERROR;
        }
        break;

      default:
        return SYNTAX_ERROR;
    }

    if (req->beg < (double) 0.0 || req->end < (double) 0.0) return SYNTAX_ERROR;

    if (req->end <= req->beg) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "illegal TIME values (end <= beg)\n");
        ++req->errors;
        ++errors;
    }

    have_time = 1;

    return 0;
}

int rd_stalst(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *kivma = "KIVMA";
int i, j, nsta;
char *sta[DRM_MAXSTA];
struct drm_cnf *cnf;
int (*compare_fnc)();
static char *fid = "rd_stalst";

    if (argc < 2) return SYNTAX_ERROR;

    req->nsta = 0; /* overwrite any preceeding list */

    cnf = drm_cnf();

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "KVAR") == 0) argv[i] = kivma;
        for (nsta = 0, j = 0; j < cnf->nsta; j++) {
            if (fnmatch(argv[i], cnf->sta[j].name, FNM_CASEFOLD) == 0) {
                if (nsta == DRM_MAXSTA) {
                    util_log(1, "%s: increase DRM_MAXSTA", fid);
                    drm_exit(DRM_BAD);
                }
                sta[nsta++] = cnf->sta[j].name;
            }
        }
        if ((req->nsta + nsta) >= DRM_MAXSTA) {
            util_log(1, "%s: increase DRM_MAXSTA", fid);
            return -1;
        }
        for (j = 0; j < nsta; j++) {
            if ((req->sta[req->nsta++] = strdup(sta[j])) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                return -2;
            }
        }
    }

/* Remove any duplicate entries in the station list */

    compare_fnc = str_compare;
    qsort(req->sta, req->nsta, sizeof(char *), compare_fnc);
    for (i = 1; i < req->nsta; i++) {
        if (strcasecmp(req->sta[i], req->sta[i-1]) == 0) {
            for (j = i; j < req->nsta - 1; j++) {
                req->sta[j] = req->sta[j+1];
            }
            --req->nsta;
        }
    }

    return 0;
}

int rd_chanlst(int argc, char **argv, struct autodrm_request *req)
{
int i, j, first, missing, ista, n, nwant, nhave;
char **want, **have;
char buffer[DRM_BUFLEN];
struct drm_datreq tmpreq;
static char *fid = "rd_chanlst";

/* Use default if no arguments given */

    if (argc == 1) {
        req->nchn   = 1;
        req->chn[0] = DRM_DEFCHNLST;
        return 0;
    } else {
        req->nchn = argc - 1;
    }

/* Station list must preceed channel list */

    if (req->nsta <= 0) {
        fprintf(efp, "  **** ERROR: empty station ");
        fprintf(efp, "list (STA_LIST with *SUPPORTED* stations ");
        fprintf(efp, "must preceed CHAN_LIST)\n");
        ++req->errors;
        ++errors;
        return 0;
    }

/* Save the list */

    for (n = 0, i = 1; i < argc; i++, n++) {
        if ((req->chn[n] = strdup(argv[i])) == NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            return -2;
        }
    }

/* Compare list of desired channels against acquired channels */

    if ((nwant = tmpreq.nchn = argc - 1) > DRM_MAXCHN) {
        util_log(1, "%s: increase DRM_MAXCHN", fid);
        return -1;
    }
    want = tmpreq.chn;

    for (ista = 0; ista < req->nsta; ista++) {
        first = 1;
        for (n = 0, i = 1; i < argc; i++, n++) tmpreq.chn[n] = argv[i];
        tmpreq.sname = req->sta[ista];
        nhave = drm_chanlst(&tmpreq, &have, buffer);
        for (i = 0; i < nwant; i++) {
            missing = 1;
            for (j = 0; j < nhave; j++) {
                if (fnmatch(want[i], have[j], FNM_CASEFOLD) == 0) missing = 0;
            }
            if (missing) {
                if (first) {
                    fprintf(efp, "  warning: sta `%s' lacks ", tmpreq.sname);
                    fprintf(efp, " the following channels:\n");
                    fprintf(efp, " ");
                    first = 0;
                }
                fprintf(efp, " %s", want[i]);
            }
        }
        if (!first) fprintf(efp, "\n");
        if (nhave < 1) {
            fprintf(efp, "  **** ERROR: ");
            fprintf(efp, "no valid channels for station `%s'\n", tmpreq.sname);
            ++req->errors;
            ++errors;
            return 0;
        }
    }

    return 0;
}

int rd_station(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
    req->type |= STATION_REQ;
    want_slist = 1;
    return 0;
}

int rd_waveform(argc, argv, req)
int argc;
char **argv;
struct autodrm_request *req;
{
static char *fid = "rd_waveform";

    if (argc < 2 ) {
        req->format = DRM_CM6;
    } else if (argc == 2) {
        if (strcasecmp(argv[1], "GSE2.0") == 0) {
            req->format = DRM_CM6;
        } else if (strcasecmp(argv[1], "SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[1], "MINISEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[1], "MINI_SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[1], "MINI-SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[1], "NATIVE") == 0) {
            req->format = DRM_NATIVE;
        } else {
            return DATA_ERROR;
        }
    } else if (argc == 3) {
        if (strcasecmp(argv[1], "GSE2.0") != 0) return SYNTAX_ERROR;
        if (strcasecmp(argv[2], "CM6") == 0) {
            req->format = DRM_CM6;
        } else if (strcasecmp(argv[2], "CM8") == 0) {
            req->format = DRM_CM8;
        } else if (strcasecmp(argv[2], "SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[2], "MINISEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[2], "MINI_SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[2], "MINI-SEED") == 0) {
            req->format = DRM_MINISEED;
        } else if (strcasecmp(argv[1], "NATIVE") == 0) {
            req->format = DRM_NATIVE;
        } else {
            return DATA_ERROR;
        }
    } else {
        return SYNTAX_ERROR;
    }

    req->type |= WAVEFORM_REQ;

    return 0;
}

int rd_stop(argc, argv, req, finfo)
int argc;
char **argv;
struct autodrm_request *req;
struct drm_finfo *finfo;
{
int i, status;
char *address;
char tmpfil[128];
char ref_id[128];
struct drm_names *name;
struct request_list *crnt, *next, *new;
static char *fid = "rd_stop";

    if (argc != 1) return SYNTAX_ERROR;

    name = drm_names();

    have_begin = 0;

/* Make sure we have a default email address */

    if (!have_email && (address = default_address()) == NULL) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "no email address for this message\n");
        ++errors;
        drm_exit(DRM_BAD);
    }

/* Satisfy HELP requests right away */

    if ((req->type & HELP_REQ) && help_not_sent) {
        want_help = 1;
        if (have_email) {
            if (req->email != NULL) {
                address = req->email;
            } else if (req->ftp != NULL) {
                address = req->ftp;
            } else {
                util_log(1, "%s: logic error: no email address", fid);
                drm_exit(DRM_BAD);
            }
        }
        drm_ack(address, DRM_HELP);
        fprintf(efp, "  <AutoDRM User's Guide sent to %s>\n", address);
        help_not_sent = 0;
    }

/*  Satisfy STATION requests right away */

    if (req->type & STATION_REQ) {
        if (have_email) {
            if (req->email != NULL) {
                address = req->email;
            } else if (req->ftp != NULL) {
                address = req->ftp;
            } else {
                util_log(1, "%s: logic error: no email address", fid);
                drm_exit(DRM_BAD);
            }
        }
        util_log(1, "sending AutoDRM Station List to %s", address);
        sprintf(tmpfil, "/var/tmp/autodrm.%d", getpid());
        if (req->msg_id != NULL) {
            sprintf(ref_id, "%s %s", req->msg_id, req->src_id);
        } else {
            sprintf(ref_id, "<none supplied>\n");
        }
        if ((status = gse_msg_init(tmpfil, finfo, ref_id)) != 0) {
            util_log(1, "%s: gse_msg_init: status %d", fid, status);
            drm_exit(DRM_BAD);
        }
        if (util_cat(tmpfil, name->canned.slist, tmpfil) != 0) {
            util_log(1, "%s: util_cat: %s %s: %s",
                fid, tmpfil, name->canned.slist, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
        if (util_email(address, "AutoDRM Station List", tmpfil) != 0) {
            util_log(1, "%s: util_email: %s", fid, syserrmsg(errno));
            drm_exit(DRM_BAD);
        } else {
            fprintf(efp, "  <AutoDRM Station List sent to %s>\n", address);
            if (unlink(tmpfil) != 0) {
                util_log(1, "%s: unlink: %s: %s", fid,tmpfil,syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        }
    }

/* Must be a WAVEFORM request at this point... make sure it is OK */

    if (!(req->type & WAVEFORM_REQ)) return 0;

    if (!have_time) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "TIME not specified\n");
        ++req->errors;
        ++errors;
    }

    if (req->nsta < 1) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "no (supported) station(s) specified\n");
        ++req->errors;
        ++errors;
    }

    if (req->nchn < 1) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "no channel(s) specified\n");
        ++req->errors;
        ++errors;
    }

    if (!have_email) {
        if (req->format == DRM_CM6) {
            if ((req->email = strdup(address)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
            fprintf(efp, "  <using %s as return address>\n", req->email);
        } else {
            if ((req->ftp = strdup(address)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
            fprintf(efp, "  <using %s as return address>\n", req->ftp);
        }
    }

    if (req->email != NULL && !AsciiOutputFormat(req->format)) {
        req->ftp = req->email;
        req->email = NULL;
        util_log(1, "switching EMAIL to FTP (binary data requested)");
    }

    if (
        req->format == DRM_MINISEED &&
        (req->linelen < SEED_MINRECEXP || req->linelen > SEED_MAXRECEXP)
    ) {
        req->linelen = 12;
    }

    if (req->errors) {
        fprintf(efp, "  <message REJECTED>\n");
        return 0;
    }

    if (nset == DRM_MAXSET) {
        util_log(1, "%s: increase DRM_MAXSET", fid);
        drm_exit(DRM_BAD);
    }

/* Add it to the list */

    new = (struct request_list *) malloc(sizeof(struct request_list));
    if (new == NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }
    new->req  = req;
    new->next = NULL;

    crnt = &head;
    while (crnt->next != NULL) crnt = crnt->next;
    crnt->next = new;

    nset += req->nsta;
    fprintf(efp, "  <message queued for processing>\n");

    return 0;
}

int drm_rdautodrm(rfp, finfo, rawreq, cnf, buffer, spam)
FILE *rfp;
struct drm_finfo *finfo;
struct drm_rawreq *rawreq;
struct drm_cnf *cnf;
char *buffer;
int *spam;
{
int lineno = 0;
int ntoken = 0;
int i, j, n, error;
char *token[MAXTOKENS];
char *copy[DRM_BUFLEN];
struct request_list *crnt;
struct autodrm_request *req;
struct drm_names *name;
static char *fid = "drm_rdautodrm";

/*  Initialize request defaults  */

    rawreq->finfo     = *finfo;
    rawreq->email     = NULL;
    rawreq->ident     = NULL;
    rawreq->aux.help  = 0;
    rawreq->aux.slist = 0;
    rawreq->aux.calib = 0;
    rawreq->nset      = 0;

/*  Prepare badreq file to report any errors  */

    name = drm_names();

    if (rename(name->msg.badreq, name->msg.tmp) != 0) {
        util_log(1, "%s: fatal error: rename: %s -> %s: %s",
            fid, name->msg.badreq, name->msg.tmp, syserrmsg(errno)
        );
        return -1;
    }
    if ((efp = fopen(name->msg.tmp, "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen, %s: %s",
            fid, name->msg.tmp, syserrmsg(errno)
        );
        return -1;
    }

/*  Read the email  */

    *spam      = 0;
    errors     = 0;
    have_begin = 0;
    want_help  = 0;
    lineno     = 0;
    reject     = 0;
    while (fgets(buffer, DRM_BUFLEN-1, rfp) != NULL) {

        ++lineno;

    /* Echo original line to error output */

        fprintf(efp, "    %s\n", buffer);

    /* Ignore lines with nothing in the first column */

        if (buffer[0] == ' ' || buffer[0] == '\n' || buffer[0] == '\t') {
            continue;
        } else if (isSpam(buffer)) {
            *spam = 1;
            for (i = strlen(buffer) - 1; i >= 0 && buffer[i] == '\n'; i--) buffer[i] = 0;
            util_log(1, "email contains SPAM");
            util_log(1, "SPAM trigger = '%s'", buffer);
            return 0;

        } else {

        /* Parse the line into individual tokens */

            ntoken = prep_line(buffer, token, MAXTOKENS, '#', " ,<>");

        /* Set default return address, if possible */

            if (strcasecmp(token[0], "REPLY-TO") == 0) {
                set_return_address(reply_to = strdup(token[1]));
            } else if (strcasecmp(token[0], "RETURN-PATH") == 0) {
                set_return_address(return_path = strdup(token[1]));
            } else if (lineno == 1 && strcasecmp(token[0], "FROM") == 0) {
                set_return_address(from = strdup(token[1]));
            }

        /* Look for stray HELP requests (outside of a formal message) */

            if (!want_help) {
                if (strcasecmp(token[0], "SUBJECT:") == 0) {
                    for (i = 1; i < ntoken; i++) {
                        if (strcasecmp(token[i], "HELP") == 0) {
                            want_help = 1;
                        }
                    }
                } else if (strcasecmp(token[0], "HELP") == 0) {
                    want_help = 1;
                } else if (strcasecmp(token[0], "GUIDE") == 0) {
                    want_help = 1;
                } else if (strncasecmp(token[0], "INFO", strlen("INFO")) == 0) {
                    want_help = 1;
                } else if (strcasecmp(token[0], "PLEASE") == 0) {
                    if (ntoken > 1 && strcasecmp(token[1], "HELP") == 0) {
                        want_help = 1;
                    }
                }
                if (want_help) {
                    if (default_address() == NULL) {
                        fflush(efp);
                        util_email(cnf->admin, "AutoDRM help w/o address!",
                            name->msg.tmp
                        );
                    } else {
                        drm_ack(default_address(), DRM_HELP);
                        help_not_sent = 0;
                    }
                }
            }

        /* If not in a request, look for a BEGIN GSE2.0 line */

            if (!have_begin) {
                if (strcasecmp(token[0], "BEGIN") == 0) {
                    if (
                        ntoken == 2 &&
                        strcasecmp(token[1], "GSE2.0") == 0
                    ) {
                        have_begin = 1;
                        have_email = 0;
                        have_time  = 0;
                        req = new_request();
                    } else {
                        fprintf(efp, "  **** ERROR illegal BEGIN statement\n");
                        ++errors;
                    }
                }

        /* Otherwise we are working a request */

            } else {

            /* Look for a supported command */

                if (strcasecmp(token[0], "MSG_TYPE") == 0) {
                    error = rd_msg_type(ntoken, token, req);
                } else if (strcasecmp(token[0], "MSG_ID") == 0) {
                    error = rd_msg_id(ntoken, token, req);
                } else if (strcasecmp(token[0], "E-MAIL") == 0) {
                    error = rd_email(ntoken, token, req);
                } else if (strcasecmp(token[0], "EMAIL") == 0) {
                    error = rd_email(ntoken, token, req);
                } else if (strcasecmp(token[0], "FTP") == 0) {
                    error = rd_ftp(ntoken, token, req);
                } else if (strcasecmp(token[0], "LINE-LEN") == 0) {
                    error = rd_linelen(ntoken, token, req);
                } else if (strcasecmp(token[0], "LINELEN") == 0) {
                    error = rd_linelen(ntoken, token, req);
                } else if (strcasecmp(token[0], "LINLEN") == 0) {
                    error = rd_linelen(ntoken, token, req);
                } else if (strcasecmp(token[0], "LIN-LEN") == 0) {
                    error = rd_linelen(ntoken, token, req);
                } else if (strcasecmp(token[0], "TIME") == 0) {
                    error = rd_time(ntoken, token, req);
                } else if (strcasecmp(token[0], "STA_LIST") == 0) {
                    error = rd_stalst(ntoken, token, req);
                } else if (strcasecmp(token[0], "CHAN_LIST") == 0) {
                    error = rd_chanlst(ntoken, token, req);
                } else if (strcasecmp(token[0], "STATION") == 0) {
                    error = rd_station(ntoken, token, req);
                } else if (strcasecmp(token[0], "WAVEFORM") == 0) {
                    error = rd_waveform(ntoken, token, req);
                } else if (strcasecmp(token[0], "STOP") == 0) {
                    error = rd_stop(ntoken, token, req, finfo);
                } else if (strcasecmp(token[0], "REF_ID") == 0) {
                    fprintf(efp, "  **** ERROR: ");
                    fprintf(efp, "%s neither expected nor allowed\n", token[0]);
                    ++req->errors;
                    ++errors;
                    reject = 1;
                } else {
                    fprintf(efp, "  <unrecognized or unsupported command ");
                    fprintf(efp, "ignored>\n");
                    error = 0;
                }

                if (error == SYNTAX_ERROR) {
                    fprintf(efp, "  **** ERROR: ");
                    fprintf(efp, "illegal %s syntax\n", token[0]);
                    ++req->errors;
                    ++errors;
                } else if (error == DATA_ERROR) {
                    fprintf(efp, "  **** ERROR: ");
                    fprintf(efp, "illegal %s value(s)", token[0]);
                    ++req->errors;
                    ++errors;
                } else if (error < 0) {
                    return error;
                }
            }
        }
    }

/* Check for final STOP */

    if (have_begin) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "missing STOP\n");
        fprintf(efp, "  <message REJECTED>\n");
        ++errors;
    }

/* At this point there must be at least 1 WAVEFORM request */

    if ((rawreq->nset = nset) < 1 && (!want_help && !want_slist)) {
        fprintf(efp, "  **** ERROR: ");
        fprintf(efp, "no valid requests found\n");
        ++errors;
    }

/* Make sure default address is not reserved */

    if ((default_email = default_address()) != NULL) {
        strcpy(buffer, default_email);
        util_ucase(buffer);
        if (
            strstr(buffer, "AUTODRM") != NULL ||
            strstr(buffer, "POSTMASTER") != NULL ||
            strstr(buffer, "MAILER-DAEMON") != NULL
        ) {
            fprintf(efp, "  **** ERROR: ");
            fprintf(efp, "illegal (reserved) default email address ");
            fprintf(efp, "(%s)\n", buffer);
            reject = 1;
        }
    }

/* If there are errors, turn error log into a GSE 2.0 message and deliver */

    fprintf(efp, "STOP\n");
    fclose(efp);

    if (reject || errors) {

        if (reject) {
            util_log(1, "email rejected");
        } else {
            util_log(1, "email contains errors");
        }

        if ((efp = fopen(name->msg.badreq, "a")) == NULL) {
            util_log(1, "%s: fatal error: %s: %s",
                fid, name->msg.badreq, syserrmsg(errno)
            );
            return -3;
        }
        fprintf(efp, "BEGIN     GSE2.0\n");
        fprintf(efp, "MSG_TYPE  data\n");
        fprintf(efp, "MSG_ID    %s %s\n", drm_mkname(finfo), cnf->dcc);
        fclose(efp);
 
        if (util_cat(name->canned.header, name->msg.badreq, NULL) != 0) {
            util_log(1, "%s: util_cat: %s, %s: %s",
                fid, name->canned.header, name->msg.badreq, syserrmsg(errno)
            );
            return -2;
        }

        if (util_cat(name->msg.tmp, name->msg.badreq, NULL) != 0) {
            util_log(1, "%s: util_cat: %s, %s: %s",
                fid, name->msg.tmp, name->msg.badreq, syserrmsg(errno)
            );
            return -2;
        }

        if (reject) {
            drm_ack(cnf->admin, DRM_DIVERTED);
            rawreq->nset = 0;
            return 0;
        } else if (default_address() == NULL) {
            util_log(1, "can't determine email address for bouncing request");
            drm_ack(cnf->admin, DRM_DIVERTED);
        } else {
            drm_ack(default_address(), DRM_BADREQ);
        }
    }

/* If there are no requests in the list, we are done */

    if (rawreq->nset == 1) {
        util_log(1, "email contains %d valid WAVEFORM request", rawreq->nset);
    } else {
        util_log(1, "email contains %d valid WAVEFORM requests", rawreq->nset);
    }
    if (rawreq->nset < 1) return 0;

/*  Convert from autodrm to idadrm format (include station splits) */

    n = 0;
    crnt = head.next;
    while (crnt != NULL) {
        req = crnt->req;
        for (i = 0; i < req->nsta; i++) {
            rawreq->set[n].finfo  = *finfo;
            if (req->ftp != NULL) {
                rawreq->email = req->ftp;
                rawreq->set[n].remote.address = req->ftp;
                rawreq->set[n].remote.agent   = DRM_WILLCALL;
            } else if (req->email != NULL) {
                rawreq->email = req->email;
                rawreq->set[n].remote.address = req->email;
                rawreq->set[n].remote.agent   = DRM_EMAIL;
            } else if (default_email != NULL) {
                rawreq->email = default_email;
                rawreq->set[n].remote.address = default_email;
                rawreq->set[n].remote.agent   = DRM_EMAIL;
            } else {
                util_log(1, "%s: can't determine return address!", fid);
                return -4;
            }
            rawreq->set[n].linelen = req->linelen;
            rawreq->set[n].email   = rawreq->email;
            rawreq->set[n].sname   = req->sta[i];
            rawreq->set[n].format  = req->format;
            rawreq->set[n].beg     = req->beg;
            rawreq->set[n].end     = req->end;
            rawreq->set[n].nset    = rawreq->nset;
            rawreq->set[n].fin     = NULL;
            rawreq->set[n].chnlst  = NULL;
            rawreq->set[n].nchn    = req->nchn;
            for (j = 0; j < req->nchn; j++) rawreq->set[n].chn[j] = req->chn[j];
            if (req->msg_id != NULL) {
                sprintf(buffer, "%s %s", req->msg_id, req->src_id);
            } else {
                sprintf(buffer, "<null> <null>");
            }
            if ((rawreq->set[n].ident = strdup(buffer)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                return -3;
            }
            rawreq->set[n].event.evla  = -12345.0;
            rawreq->set[n].event.evlo  = -12345.0;
            rawreq->set[n].event.evdp  = -12345.0;
            rawreq->set[n].event.mag   = -12345.0;
            rawreq->set[n].event.gcarc = -12345.0;
            rawreq->set[n].event.az    = -12345.0;
            rawreq->set[n].finfo.set = ++n;
        }

        crnt = crnt->next;
    }

    if (n != rawreq->nset) {
        util_log(1, "%s: logic error: %d != %d (n != rawreq->nset)",
            fid, n, rawreq->nset
        );
        drm_exit(DRM_BAD);
    }

    return 0;
}

/* Revision History
 *
 * $Log: rdautodrm.c,v $
 * Revision 1.8  2007/11/05 20:19:16  dechavez
 * added FNM_CASEFOLD if needed
 *
 * Revision 1.7  2004/05/05 16:51:46  dechavez
 * updated spam detection triggers
 *
 * Revision 1.6  2004/04/26 20:50:23  dechavez
 * added additional spam tests
 *
 * Revision 1.5  2004/01/29 23:56:28  dechavez
 * fixed error parsing CHAN_LIST
 *
 * Revision 1.4  2004/01/23 22:22:16  dechavez
 * added support for DRM_NATIVE format requests, and use AsciiOutputFormat()
 * to test for output type instead of explicit type compares
 *
 * Revision 1.3  2003/11/21 20:20:26  dechavez
 * include local "fnmatch.h"
 *
 * Revision 1.2  2003/04/24 00:10:37  dechavez
 * Anti-spam support introduced
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
