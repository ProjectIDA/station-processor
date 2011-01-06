#pragma ident "$Id: cfg.c,v 1.8 2008/03/11 20:48:45 dechavez Exp $"
/*======================================================================
 * 
 * Q330 cfg file support
 *
 *====================================================================*/
#include "q330.h"

#define DELIMITERS " =\t"
#define MAX_TOKEN  32
#define MAXLINELEN 256
#define COMMENT    '#'
#define QUOTE      '"'

static void ClearAddr(Q330_CFG *cfg)
{
    cfg->naddr = 0;
    free(cfg->addr);
    cfg->addr = NULL;
}

static void ClearDetector(Q330_CFG *cfg)
{
    cfg->ndetector = 0;
    free(cfg->detector);
    cfg->detector = NULL;
}

static void ClearCfg(Q330_CFG *cfg)
{
    if (cfg == NULL) return;
    ClearAddr(cfg);
    ClearDetector(cfg);
}

void q330DestroyCfg(Q330_CFG *cfg)
{
int i;

    if (cfg == NULL) return;
    ClearCfg(cfg);
    free(cfg);
}

static Q330_CFG *CreateCfg(char *fname)
{
Q330_CFG *cfg;

    if ((cfg = (Q330_CFG *) malloc(sizeof(Q330_CFG))) == NULL) return NULL;

    strlcpy(cfg->fname, fname, MAXPATHLEN+1);
    cfg->naddr = 0;
    cfg->addr = NULL;
    cfg->ndetector = 0;
    cfg->detector = NULL;

    return cfg;
}

static Q330_CFG *ReadCfgCleanReturn(FILE *fp, Q330_CFG *cfg, LNKLST *list, Q330_CFG *retval)
{
    if (fp != NULL) fclose(fp);
    if (cfg != NULL) q330DestroyCfg(cfg);
    if (list != NULL) listDestroy(list);

    return retval;
}

static BOOL AddAddr(LNKLST *list, char **token, int ntoken)
{
Q330_ADDR addr;

    if (ntoken != 4) {
        errno = EINVAL;
        return FALSE;
    }
    strlcpy(addr.name, token[1], MAXPATHLEN+1);
    addr.serialno = (UINT64) strtoll(token[2], NULL, 16);
    addr.authcode = (UINT64) strtoll(token[3], NULL, 16);
    if (!listAppend(list, &addr, sizeof(Q330_ADDR))) return FALSE;
    
    return TRUE;
}

static BOOL AddDetector(LNKLST *list, char **token, int ntoken)
{
Q330_DETECTOR detector;

    if (ntoken < 7) {
        errno = EINVAL;
        return FALSE;
    }

    strlcpy(detector.name, token[1], ISI_STALEN+1);
    strlcpy(detector.channels, token[2], MAXPATHLEN+1);
    detector.votes = atol(token[3]);
    detector.pre = (UINT32) atol(token[4]);
    detector.pst = (UINT32) atol(token[5]);

    if (strcasecmp(token[6], "stalta") == 0) {
        if (ntoken != 11) {
            errno = EINVAL;
            return FALSE;
        }
        detector.engine.type = DETECTOR_TYPE_STALTA;
        detector.engine.stalta.config.len.sta = (UINT32) atol(token[7]);
        detector.engine.stalta.config.len.lta = (UINT32) atol(token[8]);
        detector.engine.stalta.config.ratio.on = (REAL32) atof(token[9]);
        detector.engine.stalta.config.ratio.off = (REAL32) atof(token[10]);
        detector.enabled = TRUE;
    } else {
        errno = EINVAL;
        return FALSE;
    }

    if (!listAppend(list, &detector, sizeof(Q330_DETECTOR))) return FALSE;
    
    return TRUE;
}

static BOOL AddCalib(QDP_TYPE_C1_QCAL *calib, char **token, int ntoken)
{
    if (ntoken != 9) {
        errno = EINVAL;
        return FALSE;
    }

    calib->waveform = (UINT16) atoi(token[1]);
    calib->amplitude = (UINT16) atoi(token[2]);
    calib->duration = (UINT16) atoi(token[3]);
    calib->settle = (UINT16) atoi(token[4]);
    calib->trailer = (UINT16) atoi(token[5]);
    calib->chans = (UINT16) atoi(token[6]);
    calib->monitor = (UINT16) atoi(token[7]);
    calib->divisor = (UINT16) atoi(token[8]);

    if (calib->waveform > 4) {
        errno = EINVAL;
        return FALSE;
    }

	if (calib->duration > Q330_MAX_CAL_DURATION) calib->duration = Q330_MAX_CAL_DURATION;
    if (calib->divisor > Q330_MAX_CAL_DIVISOR) calib->divisor = Q330_MAX_CAL_DIVISOR;

    return TRUE;
}

Q330_CFG *q330ReadCfg(char *name)
{
int i, status, ntoken;
FILE *fp = NULL;
Q330_CFG *cfg = NULL;
QDP_TYPE_C1_QCAL DefaultCalib = Q330_DEFAULT_CALIB;
char *token[MAX_TOKEN];
char input[MAXLINELEN];
LNKLST *addr = NULL, *detector = NULL;
static char *DefaultName = Q330_DEFAULT_CFG_PATH;

/* Locate and open the file */

    if (name == NULL) name = getenv(Q330_CFG_ENV_STRING);
    if (name == NULL) name = DefaultName;

    if (!utilFileExists(name)) return NULL;

    if ((fp = fopen(name, "r")) == NULL) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

/* Create the config structure and save the name */

    if ((cfg = CreateCfg(name)) == NULL) return ReadCfgCleanReturn(fp, cfg, addr, NULL);
    cfg->calib = DefaultCalib;

/* Initialize the temp lists */

    if ((addr = listCreate()) == NULL) return ReadCfgCleanReturn(fp, cfg, addr, NULL);
    if ((detector = listCreate()) == NULL) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

/* Read the contents of the file into the list */

    while ((status = utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL)) == 0) {
        ntoken = utilParse(input, token, " \t", MAX_TOKEN, QUOTE);

    /* "q330" entries define Q330_ADDR structures */

        if (strcasecmp(token[0], "q330") == 0) {

            if (!AddAddr(addr, token, ntoken)) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

    /* "detector" entries define Q330_ADDR structures */

        } else if (strcasecmp(token[0], "detector") == 0) {

            if (!AddDetector(detector, token, ntoken)) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

    /* "calib" entries define QDP_TYPE_C1_QCAL structures */

        } else if (strcasecmp(token[0], "calib") == 0) {

            if (!AddCalib(&cfg->calib, token, ntoken)) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

    /* anything else is not supported */

        } else {
            errno = EINVAL;
            return ReadCfgCleanReturn(fp, cfg, addr, NULL);
        }
    }

    if (status != 1) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

/* Copy the addresses into the output structure */

    if (!listSetArrayView(addr)) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

    cfg->naddr = addr->count;
    if ((cfg->addr = (Q330_ADDR *) malloc(sizeof(Q330_ADDR) * cfg->naddr)) == NULL) return ReadCfgCleanReturn(fp, cfg, addr, NULL);
    for (i = 0; i < addr->count; i++) cfg->addr[i] = *((Q330_ADDR *) addr->array[i]);

/* Copy the detectors into the output structure */

    if (!listSetArrayView(detector)) return ReadCfgCleanReturn(fp, cfg, addr, NULL);

    cfg->ndetector = detector->count;
    if ((cfg->detector = (Q330_DETECTOR *) malloc(sizeof(Q330_DETECTOR) * cfg->ndetector)) == NULL) return ReadCfgCleanReturn(fp, cfg, detector, NULL);
    for (i = 0; i < detector->count; i++) cfg->detector[i] = *((Q330_DETECTOR *) detector->array[i]);

/* All done, return the newly created structure */

    return ReadCfgCleanReturn(fp, NULL, addr, cfg);
}

BOOL q330LookupAddr(char *name, Q330_CFG *cfg, Q330_ADDR *addr)
{
int i;

    if (name == NULL || cfg == NULL || addr == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (i = 0; i < cfg->naddr; i++) {
        if (strcmp(name, cfg->addr[i].name) == 0) {
            *addr = cfg->addr[i];
            return TRUE;
        }
    }

    errno = ENOENT;
    return FALSE;
}

BOOL q330LookupDetector(char *name, Q330_CFG *cfg, Q330_DETECTOR *detector)
{
int i;
Q330_DETECTOR *match = NULL;

    if (cfg == NULL || detector == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Search the list for an exact match */

    if (name != NULL) {
        for (i = 0; match == NULL && i < cfg->ndetector; i++) {
            if (strcmp(name, cfg->detector[i].name) == 0) match = &cfg->detector[i];
        }
    }

/* Or the default */

    for (i = 0; match == NULL && i < cfg->ndetector; i++) {
        if (strcmp(Q330_DEFAULT_SITE_NAME, cfg->detector[i].name) == 0) match = &cfg->detector[i];
    }

    if (match != NULL) {
        *detector = *match;
        return TRUE;
    } else {
        detector->enabled = FALSE;
        return TRUE;
    }
}

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
 * $Log: cfg.c,v $
 * Revision 1.8  2008/03/11 20:48:45  dechavez
 * silently ensure calib duration does not exceed hardware max
 *
 * Revision 1.7  2007/12/20 23:08:47  dechavez
 * Silently ensure calib divisor does not exceed hardware max
 *
 * Revision 1.6  2007/12/14 21:35:28  dechavez
 * added calib support
 *
 * Revision 1.5  2007/10/31 17:16:01  dechavez
 * q330LookupAddr() changed to pass Q330_ADDR
 *
 * Revision 1.4  2007/09/26 23:05:58  dechavez
 * set errno correctly when unable to find a cfg file, fix q330LookupDetector() return code when lookup fails
 *
 * Revision 1.3  2007/09/25 20:54:46  dechavez
 * cleaned up detector initialization so it works OK with libdetect 1.0.0
 *
 * Revision 1.2  2007/09/22 02:37:02  dechavez
 * added detector support
 *
 * Revision 1.1  2007/09/06 18:25:09  dechavez
 * created
 *
 */
